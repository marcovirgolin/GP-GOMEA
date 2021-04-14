#ifndef NSGA2GENERATIONHANDLER_H
#define NSGA2GENERATIONHANDLER_H

#include "GPGOMEA/Evolution/GenerationHandler.h"  
#include "GPGOMEA/Genotype/Node.h"
#include "GPGOMEA/Utils/ConfigurationOptions.h"
#include "GPGOMEA/Selection/TournamentSelection.h"
#include "GPGOMEA/Variation/SubtreeVariator.h"
#include "GPGOMEA/Semantics/SemanticBackpropagator.h"
#include "GPGOMEA/Semantics/SemanticLibrary.h"

#include <armadillo>
#include <vector>
#include <unordered_map>

class NSGA2GenerationHandler : public GenerationHandler {
public:

    NSGA2GenerationHandler(ConfigurationOptions * conf, TreeInitializer * tree_initializer, Fitness * fitness, SemanticLibrary * semlib = NULL, SemanticBackpropagator * semback = NULL) : 
        GenerationHandler(conf, tree_initializer, fitness, semlib, semback) {}; // inherit same constructor


    virtual void PerformGeneration(std::vector<Node *> & population) override;

    std::vector<std::vector<Node*>> FastNonDominatedSorting(std::vector<Node*> & population);
    void ComputeCrowndingDistance(std::vector<Node *> & front);

private:

};

#endif /* GENERATIONHANDLER_H */

