/*
 


 */

/* 
 * File:   AccuracyFitness.cpp
 * Author: virgolin
 *
 * Created on May 31, 2019, 12:25 PM
 */


#include "GPGOMEA/Fitness/AccuracyFitness.h"

void AccuracyFitness::SetFitnessCases(const arma::mat& X, FitnessCasesType fct) {

    arma::vec Y = X.col(X.n_cols - 1);
    arma::mat Xx = X;
    Xx.shed_col(Xx.n_cols - 1);


    if (fct == FitnessCasesTRAIN) {

        class_count_map.clear();
        weighted_denom = 0;

        TrainY = Y;
        TrainX = Xx;

        for (size_t i = 0; i < Y.n_elem; i++) {
            class_count_map[Y[i]] = class_count_map[Y[i]] + 1;
        }

        class_weights = arma::vec(class_count_map.size(), arma::fill::ones);

        if (use_weighting) {
            for (auto it = class_count_map.begin(); it != class_count_map.end(); it++) {
                class_weights[ it->first ] = 1.0 * (Y.n_elem - it->second) / Y.n_elem;
                weighted_denom += it->second * class_weights[it->first];
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

void AccuracyFitness::SetCustomWeights(const arma::vec& custom_weights) {
    assert(custom_weights.n_elem == class_weights.n_elem);
    for (size_t i = 0; i < class_weights.n_elem; i++) {
        class_weights[i] = custom_weights[i];
    }
    weighted_denom = 0;
    for (auto it = class_count_map.begin(); it != class_count_map.end(); it++) {
        weighted_denom += it->second * class_weights[it->first];
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

    arma::vec Pr = arma::round(P);
    arma::vec res = Y - Pr;
    double_t result = 0;

    for (size_t i = 0; i < res.n_elem; i++) {
        if (res[i] == 0) {
            result = result + class_weights[ Pr[i] ];
        }
    }

    if (use_weighting) {
        result /= weighted_denom;
    } else
        result /= Y.n_elem;

    result = 1.0 - result;
    return result;
}
