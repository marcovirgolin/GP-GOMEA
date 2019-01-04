/*
 


 */

/* 
 * File:   GOMEAGenerationHandler.h
 * Author: virgolin
 *
 * Created on June 28, 2018, 5:35 PM
 */

#ifndef GOMEAGENERATIONHANDLER_H
#define GOMEAGENERATIONHANDLER_H

#include "../Evolution/GenerationHandler.h"
#include "../GOMEA/GOMEAFOS.h"
#include "../GOMEA/GOMVariator.h"
#include "../Evolution/PopulationInitializer.h"

#include <armadillo>

class GOMEAGenerationHandler : public GenerationHandler {
public:

    GOMEAGenerationHandler(ConfigurationOptions * conf, TreeInitializer * tree_initializer, Fitness * fitness, SemanticLibrary * semlib = NULL, SemanticBackpropagator * semback = NULL)
    : GenerationHandler(conf, tree_initializer, fitness, semlib, semback) {
        linkage_normalization_matrix = new arma::mat();
    };

    virtual ~GOMEAGenerationHandler() {
        if (linkage_normalization_matrix)
            delete linkage_normalization_matrix;
    }

    void PerformGeneration(std::vector<Node *> & population) override;

    bool CheckPopulationConverged(const std::vector<Node*>& population) override;

    arma::mat * linkage_normalization_matrix = NULL;
    bool gomea_converged = false;

private:

};

#endif /* GOMEAGENERATIONHANDLER_H */

