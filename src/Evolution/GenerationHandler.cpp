/*
 


 */

/* 
 * File:   GenerationHandler.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 12:09 PM
 */

#include "GPGOMEA/Evolution/GenerationHandler.h"

using namespace std;
using namespace arma;

void GenerationHandler::PerformGeneration(std::vector<Node*> & population) {

    // Selection
    // Classic selection with replacement
    std::vector<Node *> selected_parents;
    selected_parents.reserve(population.size());
    for (size_t i = 0; i < population.size(); i++) {
        selected_parents.push_back(TournamentSelection::GetTournamentSelectionWinner(population, conf->tournament_selection_size));
    }
    // Alternative: pop-wise tournament selection that visits all population members
    //std::vector<Node *> selected_parents = TournamentSelection::PopulationWiseTournamentSelection(population, population.size(), conf->tournament_selection_size);

    // Variation
    vector<Node*> offspring = MakeOffspring(population, selected_parents);

    // Update population
    for (size_t i = 0; i < population.size(); i++) {
        population[i]->ClearSubtree();
    }
    population = offspring;

    // Update fitness
    fitness->GetPopulationFitness(population, true, conf->caching);
}

std::vector<Node*> GenerationHandler::MakeOffspring(const std::vector<Node *> & population, const vector<Node*> & selected_parents) {

    std::vector<Node *> offspring(population.size(), NULL);

    // Variation
    size_t offspring_size, variator_limit;
#pragma omp parallel
    {
        size_t offspring_size_pvt = 0;
        size_t variator_limit_pvt = population.size() * conf->subtree_crossover_proportion;
        size_t population_chunk, population_chunk_start_idx, max_attempts_variation, attempts_variation;

        max_attempts_variation = 10;
        attempts_variation = 0;

        population_chunk = (variator_limit_pvt / omp_get_num_threads());
        population_chunk_start_idx = population_chunk * omp_get_thread_num();

        // crossover
#pragma omp for schedule(static)
        for (size_t i = 0; i < variator_limit_pvt; i += 2) {
            pair<Node*, Node*> oo = SubtreeVariator::SubtreeCrossover(*selected_parents[i], *selected_parents[population_chunk_start_idx + randu() * population_chunk], conf->uniform_depth_variation, conf->caching);
            offspring[i] = oo.first;
            if (!ValidateOffspring(offspring[i], conf->maximum_tree_height, conf->maximum_solution_size)) {
                offspring[i]->ClearSubtree();
                offspring[i] = selected_parents[i]->CloneSubtree();
            }
            if (i < variator_limit_pvt) {
                offspring[i + 1] = oo.second;
                if (!ValidateOffspring(offspring[i + 1], conf->maximum_tree_height, conf->maximum_solution_size)) {
                    offspring[i + 1]->ClearSubtree();
                    offspring[i + 1] = selected_parents[i + 1]->CloneSubtree();
                }
            } else
                oo.second->ClearSubtree();
        }
        offspring_size_pvt = variator_limit_pvt;

        // mutation
        variator_limit_pvt = min((size_t) (variator_limit_pvt + population.size() * conf->subtree_mutation_proportion), (size_t) population.size());
#pragma omp for schedule(static)
        for (size_t i = offspring_size_pvt; i < variator_limit_pvt; i++) {
            while (offspring[i] == NULL) {
                offspring[i] = SubtreeVariator::SubtreeMutation(*selected_parents[i], *tree_initializer, conf->functions, conf->terminals, conf->initial_maximum_tree_height, conf->uniform_depth_variation, conf->caching);
                if (!ValidateOffspring(offspring[i], conf->maximum_tree_height, conf->maximum_solution_size)) {
                    offspring[i]->ClearSubtree();
                    offspring[i] = NULL;
                    attempts_variation++;
                    if (attempts_variation >= max_attempts_variation)
                        offspring[i] = selected_parents[i]->CloneSubtree();
                }
            }
        }
        offspring_size_pvt = variator_limit_pvt;

        // RDO
        variator_limit_pvt = min((size_t) (variator_limit_pvt + population.size() * conf->rdo_proportion), (size_t) population.size());
#pragma omp parallel for schedule(static)
        for (size_t i = offspring_size_pvt; i < variator_limit_pvt; i++) {
            while (offspring[i] == NULL) {
                offspring[i] = SubtreeVariator::SubtreeRDO(*selected_parents[i], conf->maximum_tree_height, fitness->TrainX, fitness->TrainY, *semlib, conf->uniform_depth_variation, conf->caching, conf->linear_scaling);
                if (!ValidateOffspring(offspring[i], conf->maximum_tree_height, conf->maximum_solution_size)) {
                    offspring[i]->ClearSubtree();
                    offspring[i] = NULL;
                    attempts_variation++;
                    if (attempts_variation >= max_attempts_variation)
                        offspring[i] = selected_parents[i]->CloneSubtree();
                }
            }
        }
        offspring_size_pvt = variator_limit_pvt;

        // AGX
        variator_limit_pvt = min((size_t) (variator_limit_pvt + population.size() * conf->agx_proportion), (size_t) population.size());
#pragma omp parallel for schedule(static)
        for (size_t i = offspring_size_pvt; i < variator_limit_pvt; i += 2) {
            pair<Node*, Node*> oo = SubtreeVariator::SubtreeAGX(*selected_parents[i], *selected_parents[randu() * selected_parents.size()], conf->maximum_tree_height, fitness->TrainX, *semlib, conf->uniform_depth_variation, conf->caching, conf->linear_scaling);
            offspring[i] = oo.first;
            if (!ValidateOffspring(offspring[i], conf->maximum_tree_height, conf->maximum_solution_size)) {
                offspring[i]->ClearSubtree();
                offspring[i] = selected_parents[i]->CloneSubtree();
            }
            if (i < variator_limit_pvt) {
                offspring[i + 1] = oo.second;
                if (!ValidateOffspring(offspring[i + 1], conf->maximum_tree_height, conf->maximum_solution_size)) {
                    offspring[i + 1]->ClearSubtree();
                    offspring[i + 1] = selected_parents[i + 1]->CloneSubtree();
                }
            } else
                oo.second->ClearSubtree();
        }
        offspring_size_pvt = variator_limit_pvt;

        // Reproduction
        variator_limit_pvt = min((size_t) (variator_limit_pvt + population.size() * conf->reproduction_proportion), (size_t) population.size());
#pragma omp for schedule(static)
        for (size_t i = offspring_size_pvt; i < variator_limit_pvt; i++) {
            offspring[i] = selected_parents[i]->CloneSubtree();
        }
        offspring_size_pvt = variator_limit_pvt;

#pragma omp single 
        {
            offspring_size = offspring_size_pvt;
            variator_limit = variator_limit_pvt;
        }

        // stuff that should be applied to all offspring
        // coefficient random mutation
        if (conf->coeff_mut_prob != 0) {
#pragma omp for schedule(static)
            for (size_t i = 0; i < population.size(); i++) {
                SubtreeVariator::RandomCoefficientMutation(offspring[i], conf->coeff_mut_prob, conf->coeff_mut_strength, conf->caching);
            }
        }
    }

    // elitism
    size_t actual_elitism = max(conf->elitism, population.size() - offspring_size); // e.g., when crossover delivers 1 child less

    if (actual_elitism > 0) {
        vector<size_t> elitism_indices;
        elitism_indices.reserve(actual_elitism);
        // Add indices of not initialized offspring
        for (size_t i = variator_limit; i < population.size(); i++)
            elitism_indices.push_back(i);
        // If more elites need to be used, random members of the offspring will be replaced
        if (elitism_indices.size() < actual_elitism)
            for (size_t i = 0; i < actual_elitism; i++)
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

    return offspring;
}

bool GenerationHandler::ValidateOffspring(Node* offspring, int max_height, int  max_size) {

    if (max_height > -1) {
        if (offspring->GetHeight() > max_height) {
            return false;
        }
    }

    if (max_size > -1) {
        size_t size = offspring->GetSubtreeNodes(true).size();
        if (size > max_size) {
            return false;
        }
    }

    return true;
}

bool GenerationHandler::CheckPopulationConverged(const std::vector<Node*>& population) {
    return false;
}
