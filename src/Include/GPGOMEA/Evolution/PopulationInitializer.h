/*
 


 */

/* 
 * File:   PopulationInitializer.h
 * Author: virgolin
 *
 * Created on June 28, 2018, 11:32 AM
 */

#ifndef POPULATIONINITIALIZER_H
#define POPULATIONINITIALIZER_H

#include <armadillo>

#include "GPGOMEA/Variation/TreeInitializer.h"
#include "GPGOMEA/Utils/ConfigurationOptions.h"


class PopulationInitializer {
    
public:
    
    static std::vector<Node *> InitializeTreePopulation(const ConfigurationOptions & conf, const TreeInitializer & tree_initializer, const Fitness & fitness);

    static Node * EnsureSyntacticUniqueness(Node* n, const std::vector<Node*> & pop, const ConfigurationOptions & conf, const TreeInitializer & tree_initializer, size_t height, size_t & tries);
    static Node * EnsureSemanticUniqueness(Node* n, const std::vector<Node*> & pop, const ConfigurationOptions & conf, const TreeInitializer & tree_initializer,  size_t height, size_t & tries, const Fitness & fitness);
private:
    

};

#endif /* POPULATIONINITIALIZER_H */

