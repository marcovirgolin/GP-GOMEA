/*
 


 */

/* 
 * File:   GenerationHandler.h
 * Author: virgolin
 *
 * Created on June 28, 2018, 12:08 PM
 */

#ifndef GENERATIONHANDLER_H
#define GENERATIONHANDLER_H

#include "../Genotype/Node.h"
#include "../Utils/ConfigurationOptions.h"
#include "../Selection/TournamentSelection.h"
#include "../Variation/SubtreeVariator.h"
#include "../Semantics/SemanticBackpropagator.h"
#include "../Semantics/SemanticLibrary.h"

#include <armadillo>
#include <vector>

class GenerationHandler {
public:

    GenerationHandler(ConfigurationOptions * conf, TreeInitializer * tree_initializer, Fitness * fitness, SemanticLibrary * semlib = NULL, SemanticBackpropagator * semback = NULL) {
        this->conf = conf;
        this->tree_initializer = tree_initializer;
        this->fitness = fitness;
        this->semlib = semlib;
        this->semback = semback;
    };

    virtual void PerformGeneration(std::vector<Node *> & population);
    
    virtual bool CheckPopulationConverged(const std::vector<Node*>& population);

    ConfigurationOptions * conf;
    TreeInitializer * tree_initializer;
    Fitness * fitness;
    SemanticLibrary * semlib;
    SemanticBackpropagator * semback;

private:

    std::vector<Node*> MakeOffspring(const std::vector<Node *> & population, const std::vector<Node*> & parents);
    bool ValidateOffspring(Node * offspring, int max_height, int max_size);

};

#endif /* GENERATIONHANDLER_H */

