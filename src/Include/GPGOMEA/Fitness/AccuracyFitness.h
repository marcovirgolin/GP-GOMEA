/*
 


 */

/* 
 * File:   AccuracyFitness.h
 * Author: virgolin
 *
 * Created on May 31, 2019, 12:25 PM
 */

#ifndef ACCURACYFITNESS_H
#define ACCURACYFITNESS_H

#include "GPGOMEA/Fitness/Fitness.h"

#include <map>
#include <armadillo>

class AccuracyFitness : public Fitness {
public:

    virtual void SetFitnessCases(const arma::mat & X, FitnessCasesType fct) override;

    void SetCustomWeights(const arma::vec & custom_weights);

    double_t ComputeFitness(Node * n, bool use_caching) override;

    double_t GetTestFit(Node * n) override;

    double_t GetValidationFit(Node * n) override;

    bool use_weighting = false;


private:

    arma::vec class_weights;
    double_t weighted_denom;
    std::map<size_t, size_t> class_count_map;


    double_t ComputeWeightedAccuracy(const arma::vec & P, const arma::vec & Y);

};

#endif /* ACCURACYFITNESS_H */

