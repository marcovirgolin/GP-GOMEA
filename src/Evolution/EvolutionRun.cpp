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

    // perform generation
    generation_handler->PerformGeneration(population);

    // update stats
    pop_fitnesses = fitness->GetPopulationFitness(population, false, config->caching);
    Node * best = population[ index_min(pop_fitnesses) ]; 
    size_t best_size = best->GetSubtreeNodes(true).size();
    double_t best_fit = best->cached_fitness;
    
    if (best_fit < elitist_fit) {
        elitist_fit = best_fit;
        if (elitist)
            elitist->ClearSubtree();
        elitist = best->CloneSubtree();
        elitist_size = best_size;
    }

}
