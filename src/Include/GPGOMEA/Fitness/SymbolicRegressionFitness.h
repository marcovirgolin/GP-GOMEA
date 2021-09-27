/*
 


 */

/* 
 * File:   SymbolicRegressionFitness.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 6:23 PM
 */

#ifndef SYMBOLICREGRESSIONFITNESS_H
#define SYMBOLICREGRESSIONFITNESS_H

#include "GPGOMEA/Fitness/Fitness.h"
#include "GPGOMEA/Utils/Utils.h"

#include <armadillo>

class SymbolicRegressionFitness : public Fitness {
public:

    double_t ComputeFitness(Node * n, bool use_caching) override;
    
    double_t GetTestFit(Node * n) override;
    
    double_t GetValidationFit(Node * n) override;

    double_t ComputeMSE(const arma::vec & P, const arma::vec & Y);
    
    double_t ComputeMSE(const arma::vec & P, const arma::vec & Y, double_t a, double_t b);


    bool use_linear_scaling;

private:

};

#endif /* SYMBOLICREGRESSIONFITNESS_H */

