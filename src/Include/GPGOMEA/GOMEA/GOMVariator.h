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

#include "GPGOMEA/Genotype/Node.h"
#include "GPGOMEA/Operators/Operator.h"
#include "GPGOMEA/Fitness/Fitness.h"
#include "GPGOMEA/GOMEA/GOMEATreeInitializer.h"
#include "GPGOMEA/Utils/Exceptions.h"
#include "GPGOMEA/Semantics/SemanticLibrary.h"
#include "GPGOMEA/Semantics/SemanticBackpropagator.h"
#include "GPGOMEA/Variation/SubtreeVariator.h"

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

