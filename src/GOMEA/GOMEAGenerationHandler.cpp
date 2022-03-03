/*
 


 */

/* 
 * File:   GOMEAGenerationHandler.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 5:35 PM
 */

#include "GPGOMEA/GOMEA/GOMEAGenerationHandler.h"


using namespace std;
using namespace arma;

void GOMEAGenerationHandler::PerformGeneration(std::vector<Node*> & population) {

    // only works for population.size() > 1
    assert(population.size() > 1);

    // if using GOMEA and it converged, re-initialize it
    if (!conf->use_IMS && gomea_converged) {
        // this is going to be handled by the IMS if it is active
        for (Node * n : population) {
            n->ClearSubtree();
        }
        population.clear();
        population = PopulationInitializer::InitializeTreePopulation(*conf, *tree_initializer, *fitness);
        gomea_converged = false;
    }

    // Build FOS
    vector<vector < size_t >> FOS;
    FOS = GOMEAFOS::GenerateFOS(population, conf->fos_type, linkage_normalization_matrix, conf->gomfos_noroot);

    // Replace worst % of the population
    if (conf->gomea_replace_worst > 0) {
        vec fitnesses = fitness->GetPopulationFitness(population, false, conf->caching);
        uvec fit_order = sort_index(fitnesses);
        for (size_t i = 0; i < conf->gomea_replace_worst * population.size(); i++) {
            population[ fit_order[ population.size() - 1 - i ] ]->ClearSubtree();
            TreeInitShape tis = randu() < 0.5 ? TreeInitShape::TreeInitFULL : TreeInitShape::TreeInitGROW;
            population[ fit_order[ population.size() - 1 - i ] ] = tree_initializer->InitializeRandomTree(tis, conf->initial_maximum_tree_height, conf->functions, conf->terminals);
            fitness->ComputeFitness(population[ fit_order[ population.size() - 1 - i ] ], conf->caching);
        }
    }

    // Variate solutions
    vector<Node*> offspring(population.size());

    size_t variator_limit = population.size();
#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < variator_limit; i++) {
        // if batching, re-evaluate the parent w.r.t. current batch
        if (conf->batch_size > 0) {
            fitness->ComputeFitness(population[i], conf->caching);
        }
        // generate offspring
        Node * off;
        // apply GOM
        off = GOMVariator::GOM(*population[i], population, FOS, *fitness, 
            conf->coeff_mut_prob, conf->coeff_mut_strength, conf->gom_coeff_mut_strat,
            conf->caching);

        // check if we should do coeff mutation after GOM (note: this does not guarantee strict improvement)
        if (conf->coeff_mut_prob != 0) {
            if (conf->gom_coeff_mut_strat == GOMCoeffMutStrat::gcmsAfterOnce){
                GOMVariator::CoeffMutGOMStyle(off, 1, *fitness, conf->coeff_mut_prob, conf->coeff_mut_strength, conf->caching);
            }
            else if (conf->gom_coeff_mut_strat == GOMCoeffMutStrat::gcmsAfterFOSSizeTimes)
            {
                GOMVariator::CoeffMutGOMStyle(off, FOS.size(), *fitness, conf->coeff_mut_prob, conf->coeff_mut_strength, conf->caching);
            }
        }
        // store offspring
        offspring[i] = off;
    }
    variator_limit = population.size();

    // elitism
    if (conf->elitism > 1) {
        random_shuffle(offspring.begin(), offspring.end());

        vector<size_t> elitism_indices;
        elitism_indices.reserve(conf->elitism);
        // Add indices of not initialized offspring
        for (size_t i = variator_limit; i < population.size(); i++)
            elitism_indices.push_back(i);
        // If more elites need to be used, random members of the offspring will be replaced
        if (elitism_indices.size() < conf->elitism)
            for (size_t i = 0; i < conf->elitism; i++)
                elitism_indices.push_back(randu() * population.size());
        // sort fitnesses
        vec fitnesses = fitness->GetPopulationFitness(population, false, conf->caching);
        uvec order_fitnesses = sort_index(fitnesses);
        // insert elites
        size_t j = 0;
        for (size_t i : elitism_indices) {
            if (offspring[i])
                offspring[i]->ClearSubtree();
            offspring[i] = population[ order_fitnesses[j++] ]->CloneSubtree();
        }
    }
    assert(offspring.size() == population.size());

    // Replace population
    for (size_t i = 0; i < population.size(); i++) {
        population[i]->ClearSubtree();
    }

    population = offspring;

    if (population.size() > 1 && CheckPopulationConverged(population)) {
        cout << " ! population converged " << endl;
        gomea_converged = true;
    }

}

bool GOMEAGenerationHandler::CheckPopulationConverged(const std::vector<Node*>& population) {

    size_t idx = randu() * population.size();
    vector<Node *> rand_sol_nodes = population[ idx ]->GetSubtreeNodes(false);
    vector<bool> active_sol_nodes;
    active_sol_nodes.reserve(rand_sol_nodes.size());
    for (size_t i = 0; i < rand_sol_nodes.size(); i++)
        active_sol_nodes.push_back(rand_sol_nodes[i]->IsActive());

    for (size_t i = 0; i < population.size(); i++) {
        if (i == idx)
            continue;

        vector<Node *> nodes = population[i]->GetSubtreeNodes(false);

        for (size_t j = 0; j < nodes.size(); j++) {
            if ((nodes[j]->IsActive() != active_sol_nodes[j]) ||
                    nodes[j]->GetValue().compare(rand_sol_nodes[j]->GetValue()) != 0)
                return false;
        }
    }

    return true;

}
