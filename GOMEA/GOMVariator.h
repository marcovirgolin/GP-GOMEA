/*
 


 */

/* 
 * File:   GOMVariator.h
 * Author: virgolin
 *
 * Created on June 28, 2018, 6:07 PM
 */

#ifndef GOMVARIATOR_H
#define GOMVARIATOR_H

#include "../Genotype/Node.h"
#include "../Operators/Operator.h"
#include "../Fitness/Fitness.h"
#include "GOMEATreeInitializer.h"
#include "../Utils/Exceptions.h"
#include "../Semantics/SemanticLibrary.h"
#include "../Semantics/SemanticBackpropagator.h"
#include "../Variation/SubtreeVariator.h"

#include <vector>
#include <armadillo>

class GOMVariator {
public:

    GOMVariator() {
    };

    static Node * GOM(const Node & sol, const std::vector<Node*> & donors, const std::vector<std::vector<size_t>> &FOS, Fitness & fit, bool use_caching);
    static Node * GOM(const Node & sol,  const std::vector<Operator*> & functions, const std::vector<Operator*> & terminals, const std::vector<std::vector<size_t>> &FOS, Fitness & fit, bool use_caching);
    
    static Node * MakeBiggerGOMEATree(const Node & original, size_t orig_height, size_t desired_height,  size_t max_arity,  const GOMEATreeInitializer & tree_init, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals);
    
private:


};

#endif /* GOMVARIATOR_H */

