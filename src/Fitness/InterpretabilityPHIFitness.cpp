#include "GPGOMEA/Fitness/InterpretabilityPHIFitness.h"

using namespace std;
using namespace arma;

double_t InterpretabilityPHIFitness::ComputeFitness(Node* n, bool use_caching) {
    evaluations++;
    return ComputeInterpretabilityPHI(n);
}

double_t InterpretabilityPHIFitness::GetValidationFit(Node* n) {
    return ComputeInterpretabilityPHI(n);
}

double_t InterpretabilityPHIFitness::GetTestFit(Node* n) {
    return ComputeInterpretabilityPHI(n);
}

double_t InterpretabilityPHIFitness::ComputeInterpretabilityPHI(Node * n) {
	vector<Node*> nodes = n->GetSubtreeNodes(true);

    size_t n_nodes = nodes.size();
    size_t n_ops = 0;
    size_t n_naops = 0;

    for(Node * c : nodes) {
    	if (c->op->type == OperatorType::opFunction) {
    		n_ops++;
    		if (!c->op->is_arithmetic)
    			n_naops++;
    	}
    }
    size_t n_nacomp = n->Count_N_NaComp();

    // Model from Virgolin et al. PPSN 2020 
    double_t phi = 100*(0.00195041*n_nodes + 0.00502375*n_ops + 0.03351907*n_naops + 0.04472121*n_nacomp);
    return phi;
}