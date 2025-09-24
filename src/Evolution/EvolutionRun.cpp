/*
 


 */

/* 
 * File:   EvolutionRun.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 11:28 AM
 */

#include "GPGOMEA/Evolution/EvolutionRun.h"


using namespace std;
using namespace arma;

void EvolutionRun::Initialize() {

    elitist = NULL;

    // Initialize population
    population = PopulationInitializer::InitializeTreePopulation(*config, *tree_initializer, *fitness);

    // Compute fitness of the population
    pop_fitnesses = fitness->GetPopulationFitness(population, true, config->caching);

    // Initialize linkage normalization matrix
    if (config->gomea) {
        size_t num_nodes = population[0]->GetSubtreeNodes(false).size();
        ((GOMEAGenerationHandler*)generation_handler)->linkage_normalization_matrix = arma::mat(num_nodes, num_nodes, arma::fill::zeros);
    }

    // create semantic library if needed
    if (config->semantic_variation && config->semback_library_type == SemanticLibraryType::SemLibRandomStatic)
        semantic_library->GenerateRandomLibrary(config->semback_library_max_height, config->semback_library_max_size, *fitness, config->functions, config->terminals, *tree_initializer, config->caching);
}

void EvolutionRun::DoGeneration() {

    // create semantic library if needed
    if (config->semantic_variation) {
        if (config->semback_library_type == SemanticLibraryType::SemLibRandomDynamic)
            semantic_library->GenerateRandomLibrary(config->semback_library_max_height, config->semback_library_max_size, *fitness, config->functions, config->terminals, *tree_initializer, config->caching);
        else if (config->semback_library_type == SemanticLibraryType::SemLibPopulation)
            semantic_library->GeneratePopulationLibrary(config->semback_library_max_height, config->semback_library_max_size, population, *fitness, config->caching);
    }

    // if batching, temporarily set the batch
    arma::mat backupX;
    arma::vec backupY;
    if (generation_handler->conf->batch_size > 0 && 
        generation_handler->conf->batch_size < fitness->TrainX.n_rows) {
        backupX = arma::mat(fitness->TrainX);
        backupY = arma::vec(fitness->TrainY);
        // sample a batch
        arma::uvec samples = arma::randperm(generation_handler->conf->batch_size);
        arma::uvec unique_samples = arma::unique(samples).as_col();
        arma::mat batchX = fitness->TrainX.rows(samples);
        arma::vec batchY = fitness->TrainY.elem(samples);
        batchX.insert_cols(batchX.n_cols, batchY);
        // set as training fitness cases
        fitness->SetFitnessCases(batchX, FitnessCasesType::FitnessCasesTRAIN);
    }
   
    // perform generation
    generation_handler->PerformGeneration(population);

    // if batching, remove the batch and put the original training set back
    if (generation_handler->conf->batch_size > 0 && 
        generation_handler->conf->batch_size < fitness->TrainX.n_rows) {
        backupX.insert_cols(backupX.n_cols, backupY);
        fitness->SetFitnessCases(backupX, FitnessCasesType::FitnessCasesTRAIN);
    }

    // update stats
    pop_fitnesses = fitness->GetPopulationFitness(population, false, config->caching); //note: no recomputation of fitness here
    Node * best = population[ index_min(pop_fitnesses) ]; 
    size_t best_size = best->GetSubtreeNodes(true).size();
    double_t best_fit = best->cached_fitness;
    // if batching, actually do a full evaluation to get best_fit
    //if (generation_handler->conf->batch_size > 0) {
    //    best_fit = fitness->ComputeFitness(best, false);
    //}
    
    // see if we have a new elite
    if (generation_handler->conf->batch_size > 0) {
        // always accept the change
        elitist_fit = best_fit;
        if (elitist)
            elitist->ClearSubtree();
        elitist = best->CloneSubtree();
        elitist_size = best_size;

        // but still use check to see if fitness is decreaseing for 
        // stuff that relies num_gen_without_improvement 
        if (best_fit < elitist_fit) {
            num_generations_without_improvement = 0;
        } else {
            num_generations_without_improvement++;
        }
    } else {
        // handle classic scenario for updating the elite & counting num gen w/o improvement
        if (best_fit < elitist_fit) {
            elitist_fit = best_fit;
            if (elitist)
                elitist->ClearSubtree();
            elitist = best->CloneSubtree();
            elitist_size = best_size;
            num_generations_without_improvement = 0;
        } else {
            num_generations_without_improvement++;
        }
    }
    

    // stuff that should trigger if we are stagnating
    if (num_generations_without_improvement != 0){
        // handle coeff mutation decay
        if (generation_handler->conf->coeff_mut_num_gen_no_impr_decay > 0) {
            if(num_generations_without_improvement % generation_handler->conf->coeff_mut_num_gen_no_impr_decay == 0) {
                generation_handler->conf->coeff_mut_strength *= generation_handler->conf->coeff_mut_decay;
            }
        }
    }

    // update mo_archive
    if (is_multiobj){
        // For each solution in the population with best rank, try to fit it in the archive
        for(Node * solution : population) {
            if (solution->rank != 0)
                continue;
            // check if worth inserting in the archive_size
            bool solution_is_dominated = false;
            bool identical_objectives_already_exist;
            for(size_t i = 0; i < mo_archive.size(); i++) {
                // check domination
                Node * n = mo_archive[i];
                solution_is_dominated = n->Dominates(solution);
                if (solution_is_dominated)
                    break;

                identical_objectives_already_exist = true;
                for(size_t j = 0; j < solution->cached_objectives.n_elem; j++){
                    if (solution->cached_objectives[j] != n->cached_objectives[j]) {
                        identical_objectives_already_exist = false;
                        break;
                    }
                }
                if (identical_objectives_already_exist)
                    break;

                bool n_is_dominated = solution->Dominates(n);
                if (n_is_dominated) {
                    n->ClearSubtree();
                    mo_archive[i] = NULL;  // keep this guy 
                }
            }

            if (!solution_is_dominated && !identical_objectives_already_exist) {
                mo_archive.push_back(solution->CloneSubtree());    // clone it
            }

            vector<Node*> updated_archive; updated_archive.reserve(mo_archive.size());
            for(size_t i = 0; i < mo_archive.size(); i++)
                if (mo_archive[i])
                    updated_archive.push_back(mo_archive[i]);
            mo_archive = updated_archive;
        }
    }


}
