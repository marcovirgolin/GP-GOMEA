/*
 


 */

/* 
 * File:   EvolutionRun.h
 * Author: virgolin
 *
 * Created on June 28, 2018, 11:28 AM
 */

#ifndef EVOLUTIONRUN_H
#define EVOLUTIONRUN_H


#include "EvolutionState.h"
#include "PopulationInitializer.h"
#include "../Utils/Logger.h"
#include "../GOMEA/GOMEAGenerationHandler.h"
#include "GenerationHandler.h"

#include <iostream>
#include <vector>

class EvolutionRun {
public:

    EvolutionRun(EvolutionState & st) {
        config = new ConfigurationOptions(*st.config); // clone of the configuration settings
        if (st.semantic_library)
            semantic_library = new SemanticLibrary(*st.semantic_library); // clone semantic library

        if (st.config->gomea) {
            generation_handler = new GOMEAGenerationHandler(*((GOMEAGenerationHandler*) st.generation_handler)); // clone generation handler
            if (((GOMEAGenerationHandler*) st.generation_handler)->linkage_normalization_matrix)
                ((GOMEAGenerationHandler*) generation_handler)->linkage_normalization_matrix = new arma::mat(); // detach pointer to previous linkage normalization matrix
            ((GOMEAGenerationHandler*) generation_handler)->gomea_converged = false;
        } else {
            generation_handler = new GenerationHandler(*st.generation_handler);
            if (st.semantic_library)
                generation_handler->semlib = semantic_library;
        }
     
        // set correct handler to config
        generation_handler->conf = config;

        tree_initializer = st.tree_initializer; // share same tree initializer
        fitness = st.fitness; // share same fitness
    };

    virtual ~EvolutionRun() {
        for (Node * n : population) {
            n->ClearSubtree();
        }

        if (elitist)
            elitist->ClearSubtree();

        delete config;
        if (semantic_library)
            delete semantic_library;
        delete generation_handler;
    }

    void Initialize();
    void DoGeneration();

    std::vector<Node*> population;
    ConfigurationOptions * config = NULL;
    TreeInitializer * tree_initializer = NULL;
    GenerationHandler * generation_handler = NULL;
    Fitness * fitness = NULL;
    SemanticLibrary * semantic_library = NULL;

    arma::vec pop_fitnesses;

    Node * elitist = NULL;
    double_t elitist_fit = arma::datum::inf;
    size_t elitist_size;


private:

};

#endif /* EVOLUTIONRUN_H */

