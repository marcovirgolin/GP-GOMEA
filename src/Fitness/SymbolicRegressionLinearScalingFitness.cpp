/*
 


 */


#include "GPGOMEA/Fitness/SymbolicRegressionLinearScalingFitness.h"

using namespace std;
using namespace arma;

double_t SymbolicRegressionLinearScalingFitness::ComputeLinearScalingMSE(const arma::vec& P) {

    pair<double_t,double_t> ab = Utils::ComputeLinearScalingTerms(P, TrainY, &trainY_mean, &var_comp_trainY);
    
    arma::vec res;
    if (ab.second != 0) {
        res = TrainY - (ab.first + ab.second * P);
    } else {
        res = TrainY - ab.first; // do not need to multiply by P elements by 0
    }
    
    double_t ls_mse = arma::mean( arma::square(res) );
    return ls_mse;

}

double_t SymbolicRegressionLinearScalingFitness::ComputeLinearScalingMSE(const arma::vec& P, const arma::vec& Y, double_t a, double_t b) {

    arma::vec res = Y - (a + b * P);
    double_t ls_mse = arma::sum(arma::square(res)) / res.n_elem;
    return ls_mse;

}

double_t SymbolicRegressionLinearScalingFitness::GetTestFit(Node * n) {
    vec P = n->GetOutput( TrainX, false );
    pair<double_t,double_t> ab = Utils::ComputeLinearScalingTerms(P, TrainY, &trainY_mean, &var_comp_trainY);
    P = n->GetOutput(TestX, false);
    double_t fit = ComputeLinearScalingMSE(P, TestY, ab.first, ab.second);
    
    //fit = 1 - fit / as_scalar(var(TestY));
    return fit;
}

double_t SymbolicRegressionLinearScalingFitness::GetValidationFit(Node* n) {
    vec P = n->GetOutput( TrainX, false );
    pair<double_t,double_t> ab = Utils::ComputeLinearScalingTerms(P, TrainY, &trainY_mean, &var_comp_trainY);
    P = n->GetOutput(ValidationX, false);
    double_t fit = ComputeLinearScalingMSE(P, ValidationY, ab.first, ab.second);
    
    //fit = 1 - fit / as_scalar(var(TestY));
    return fit;
}


double_t SymbolicRegressionLinearScalingFitness::ComputeFitness(Node* n, bool use_caching) {
    
    evaluations++;
    
    arma::vec P = n->GetOutput(TrainX, use_caching);
    double_t fit = ComputeLinearScalingMSE(P);
    if (std::isnan(fit))
            fit = arma::datum::inf;
    n->cached_fitness = fit;
    return fit;
    
}
