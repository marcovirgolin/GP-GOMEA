/*
 


 */

/* 
 * File:   SymbolicRegressionFitness.cpp
 * Author: virgolin
 *
 * Created on June 27, 2018, 6:23 PM
 */


#include "GPGOMEA/Fitness/SymbolicRegressionFitness.h"

using namespace std;

double_t SymbolicRegressionFitness::ComputeFitness(Node* n, bool use_caching) {

    evaluations++;

    arma::vec P = n->GetOutput(TrainX, use_caching);
    double_t fit = ComputeMSE(P, TrainY);
    if (std::isnan(fit)) {
        fit = arma::datum::inf;
    }
    n->cached_fitness = fit;
    return fit;

}

double_t SymbolicRegressionFitness::GetValidationFit(Node* n) {
    double_t fit;
    if (this->use_linear_scaling){
        arma::vec P = n->GetOutput(TrainX, false);
        pair<double_t,double_t> ab = Utils::ComputeLinearScalingTerms(P, TrainY, &trainY_mean, &var_comp_trainY);
        P = n->GetOutput(ValidationX, false);
        fit = ComputeMSE(P, ValidationY, ab.first, ab.second);
    } else {
        arma::vec P = n->GetOutput(ValidationX, false);
        fit = ComputeMSE(P, ValidationY, 0.0, 1.0);
    }
    return fit;
}


double_t SymbolicRegressionFitness::GetTestFit(Node* n) {

    double_t fit;
    if (this->use_linear_scaling){
        arma::vec P = n->GetOutput(TrainX, false);
        pair<double_t,double_t> ab = Utils::ComputeLinearScalingTerms(P, TrainY, &trainY_mean, &var_comp_trainY);
        P = n->GetOutput(TestX, false);
        fit = ComputeMSE(P, TestY, ab.first, ab.second);
    } else {
        arma::vec P = n->GetOutput(TestX, false);
        fit = ComputeMSE(P, TestY, 0.0, 1.0);
    }
    return fit;

}

double_t SymbolicRegressionFitness::ComputeMSE(const arma::vec& P, const arma::vec& Y) {
    arma::vec res;
    if (this->use_linear_scaling){
        pair<double_t,double_t> ab = Utils::ComputeLinearScalingTerms(P, TrainY, &trainY_mean, &var_comp_trainY);
        if (ab.second != 0) {
            res = TrainY - (ab.first + ab.second * P);
        } else {
            res = TrainY - ab.first; // do not need to multiply by P elements by 0
        }
    } else {
        res = Y - P;
    }

    double_t mse = arma::mean( arma::square(res) );
    return mse;
}

double_t SymbolicRegressionFitness::ComputeMSE(const arma::vec& P, const arma::vec& Y, double_t a, double_t b) {
    arma::vec res = Y - (a + b * P);
    double_t ls_mse = arma::sum(arma::square(res)) / res.n_elem;
    return ls_mse;
}