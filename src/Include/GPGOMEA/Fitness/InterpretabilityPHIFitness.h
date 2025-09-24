#ifndef INTERPRETABILITYPHIFITNESS_H
#define INTERPRETABILITYPHIFITNESS_H

#include "GPGOMEA/Fitness/Fitness.h"

#include <armadillo>

class InterpretabilityPHIFitness : public Fitness {
public:

    double_t ComputeFitness(Node * n, bool use_caching) override;
    
    double_t GetTestFit(Node * n) override;
    
    double_t GetValidationFit(Node * n) override;

    double_t ComputeInterpretabilityPHI(Node * n);

private:

};

#endif /* INTERPRETABILITYPHIFITNESS_H */