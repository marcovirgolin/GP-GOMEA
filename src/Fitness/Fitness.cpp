/*
 


 */

/* 
 * File:   Fitness.cpp
 * Author: virgolin
 * 
 * Created on June 27, 2018, 6:14 PM
 */

#include "GPGOMEA/Fitness/Fitness.h"

using namespace std;
using namespace arma;

Fitness::Fitness() {
}

Fitness::Fitness(const Fitness& orig) {
}

Fitness::~Fitness() {
}

double_t Fitness::ComputeFitness(Node* n, bool use_caching) {
    throw NotImplementedException("Fitness::ComputeFitness not implemented");
}

arma::mat Fitness::ReadFitnessCases(std::string filepath) {
    arma::mat X;
    X.load(filepath);
    return X;
}

void Fitness::SetFitnessCases(const arma::mat& X, FitnessCasesType fct) {

    arma::vec Y = X.col(X.n_cols - 1);
    arma::mat Xx = X;
    Xx.shed_col(Xx.n_cols - 1);

    if (fct == FitnessCasesTRAIN) {
        TrainY = Y;
        TrainX = Xx;

        trainY_mean = arma::mean(TrainY);
        trainY_std = arma::stddev(TrainY, 1);

        var_comp_trainY = Y - trainY_mean;
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

arma::vec Fitness::GetPopulationFitness(const std::vector<Node*>& population, bool compute, bool use_caching) {

    vec fitnesses(population.size());

    if (compute) {
#pragma omp parallel for schedule(static)
        for (size_t i = 0; i < population.size(); i++) {
            fitnesses[i] = ComputeFitness(population[i], use_caching);
        }
    } else {
#pragma omp parallel for schedule(static)
        for (size_t i = 0; i < population.size(); i++) {
            fitnesses[i] = population[i]->cached_fitness;
        }
    }

    return fitnesses;
}

Node* Fitness::GetBest(const std::vector<Node*>& population, bool compute, bool use_caching) {

    vec fitnesses = GetPopulationFitness(population, compute, use_caching);
    Node * best = population[ index_min(fitnesses) ];
    return best;

}

double_t Fitness::GetTestFit(Node * n) {
    throw NotImplementedException("Fitness::GetTestFit not implemented.");
}

double_t Fitness::GetValidationFit(Node* n) {
    throw NotImplementedException("Fitness::GetValidationFit not implemented.");
}


