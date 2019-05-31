/*
 


 */

/* 
 * File:   AccuracyFitness.cpp
 * Author: virgolin
 *
 * Created on May 31, 2019, 12:25 PM
 */


#include "AccuracyFitness.h"

void AccuracyFitness::SetFitnessCases(const arma::mat& X, FitnessCasesType fct) {

    arma::vec Y = X.col(X.n_cols - 1);
    arma::mat Xx = X;
    Xx.shed_col(Xx.n_cols - 1);

    std::map<size_t, size_t> class_count_map;

    if (fct == FitnessCasesTRAIN) {
        TrainY = Y;
        TrainX = Xx;

        for (int i = 0; i < Y.n_elem; i++) {
            class_count_map[Y[i]] = class_count_map[Y[i]] + 1;
        }

        class_weights = arma::vec(class_count_map.size(), arma::fill::ones);

        if (use_weightening) {
            for (auto it = class_count_map.begin(); it != class_count_map.end(); it++) {
                class_weights[ it->first ] = 1.0 * Y.n_elem / it->second;
            }
        }

    } else if (fct == FitnessCasesTEST) {
        TestY = Y;
        TestX = Xx;
    } else if (fct == FitnessCasesVALIDATION) {
        ValidationY = Y;
        ValidationX = Xx;
    } else {
        throw std::runtime_error("Fitness::SetFitnessCases invalid fitness cases type provided.");
    }

}

double_t AccuracyFitness::ComputeFitness(Node* n, bool use_caching) {

    evaluations++;

    arma::vec P = n->GetOutput(TrainX, use_caching);
    double_t fit = ComputeWeightedAccuracy(P, TrainY);
    if (std::isnan(fit)) {
        fit = arma::datum::inf;
    }
    n->cached_fitness = fit;
    return fit;

}

double_t AccuracyFitness::GetValidationFit(Node* n) {
    arma::vec P = n->GetOutput(ValidationX, false);
    double_t fit = ComputeWeightedAccuracy(P, ValidationY);
    return fit;
}

double_t AccuracyFitness::GetTestFit(Node* n) {

    arma::vec P = n->GetOutput(TestX, false);
    double_t fit = ComputeWeightedAccuracy(P, TestY);
    return fit;

}

double_t AccuracyFitness::ComputeWeightedAccuracy(const arma::vec& P, const arma::vec& Y) {
    arma::vec res = Y - P;
    double_t result = 0;

    for (int i = 0; i < res.n_elem; i++) {

        if (res[i] == 0) {
            result = result + class_weights[P[i]];
        }

    }

    result /= Y.n_elem;
    return result ;
}
