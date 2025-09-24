#include "GPGOMEA/Fitness/MOFitness.h"

using namespace arma;
using namespace std;

MOFitness::MOFitness(std::vector<Fitness*> sub_fitness_functions) {
    this->sub_fitness_functions = sub_fitness_functions;
}

void MOFitness::SetFitnessCases(const arma::mat& X, FitnessCasesType fct) {
    Fitness::SetFitnessCases(X, fct);
    for(Fitness * fitness : sub_fitness_functions)
        fitness->SetFitnessCases(X, fct);
}

double_t MOFitness::ComputeFitness(Node* n, bool use_caching) {
    evaluations++;

    vec objs = vec(sub_fitness_functions.size());
    for(size_t i = 0; i < sub_fitness_functions.size(); i++)  {
        objs[i] = sub_fitness_functions[i]->ComputeFitness(n, use_caching);
    }

    n->cached_fitness = objs[0];
    n->cached_objectives = objs;
    return objs[0];
}

double_t MOFitness::GetValidationFit(Node* n) {
    vec objs = vec(sub_fitness_functions.size());
    for(size_t i = 0; i < sub_fitness_functions.size(); i++)  {
        objs[i] = sub_fitness_functions[i]->GetValidationFit(n);
    }
    return objs[0];
}

double_t MOFitness::GetTestFit(Node* n) {
    vec objs = vec(sub_fitness_functions.size());
    for(size_t i = 0; i < sub_fitness_functions.size(); i++)  {
        objs[i] = sub_fitness_functions[i]->GetTestFit(n);
    }
    return objs[0];
}
