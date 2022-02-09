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

#include "GPGOMEA/Evolution/GenerationHandler.h"
#include "GPGOMEA/GOMEA/GOMEAFOS.h"
#include "GPGOMEA/GOMEA/GOMVariator.h"
#include "GPGOMEA/Evolution/PopulationInitializer.h"

#include <armadillo>

class GOMEAGenerationHandler : public GenerationHandler {
public:

    GOMEAGenerationHandler(ConfigurationOptions * conf, TreeInitializer * tree_initializer, Fitness * fitness, SemanticLibrary * semlib = NULL, SemanticBackpropagator * semback = NULL)
    : GenerationHandler(conf, tree_initializer, fitness, semlib, semback) {
    };

    //virtual ~GOMEAGenerationHandler() {
    //    delete linkage_normalization_matrix;
    //}

    void PerformGeneration(std::vector<Node *> & population) override;

    bool CheckPopulationConverged(const std::vector<Node*>& population) override;

    arma::mat linkage_normalization_matrix;
    bool gomea_converged = false;

private:

};

#endif /* GOMEAGENERATIONHANDLER_H */

