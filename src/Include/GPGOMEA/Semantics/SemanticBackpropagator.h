/*
 


 */

/* 
 * File:   SemanticBackpropagator.h
 * Author: virgolin
 *
 * Created on June 30, 2018, 6:20 PM
 */

#ifndef SEMANTICBACKPROPAGATOR_H
#define SEMANTICBACKPROPAGATOR_H

#include "GPGOMEA/Genotype/Node.h"
#include "GPGOMEA/Utils/Utils.h"

#include <vector>
#include <armadillo>

class SemanticBackpropagator {
public:

    SemanticBackpropagator() {
    };

    static std::vector<arma::vec> ComputeSemanticBackpropagation(const arma::mat & X, const arma::mat & Y, Node * n, bool use_caching, bool linear_scaling = false);
    
private:

};

#endif /* SEMANTICBACKPROPAGATOR_H */

