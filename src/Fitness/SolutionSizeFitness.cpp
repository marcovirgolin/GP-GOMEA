#include "GPGOMEA/Fitness/SolutionSizeFitness.h"

double_t SolutionSizeFitness::ComputeFitness(Node* n, bool use_caching) {
    evaluations++;
    return n->GetSubtreeNodes(true).size();
}

double_t SolutionSizeFitness::GetValidationFit(Node* n) {
    return n->GetSubtreeNodes(true).size();
}

double_t SolutionSizeFitness::GetTestFit(Node* n) {
    return n->GetSubtreeNodes(true).size();
}