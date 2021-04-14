#ifndef SOLUTIONSIZEFITNESS_H
#define SOLUTIONSIZEFITNESS_H

#include "GPGOMEA/Fitness/Fitness.h"

#include <armadillo>

class SolutionSizeFitness : public Fitness {
public:

    double_t ComputeFitness(Node * n, bool use_caching) override;
    
    double_t GetTestFit(Node * n) override;
    
    double_t GetValidationFit(Node * n) override;

private:

};

#endif /* SOLUTIONSIZEFITNESS_H */