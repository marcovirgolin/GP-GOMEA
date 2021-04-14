#ifndef MOFITNESS_H
#define MOFITNESS_H

#include "GPGOMEA/Fitness/Fitness.h"

#include <armadillo>

class MOFitness : public Fitness {
public:

	MOFitness(std::vector<Fitness*> sub_fitness_functions);

	virtual void SetFitnessCases(const arma::mat & X, FitnessCasesType fct) override;

	virtual ~MOFitness() {
		for (size_t i= 0; i < sub_fitness_functions.size(); i++)
			delete sub_fitness_functions[i];
	}

    double_t ComputeFitness(Node * n, bool use_caching) override;
    
    double_t GetTestFit(Node * n) override;
    
    double_t GetValidationFit(Node * n) override;

    std::vector<Fitness*> sub_fitness_functions;

private:

};

#endif /* MOFITNESS_H */

