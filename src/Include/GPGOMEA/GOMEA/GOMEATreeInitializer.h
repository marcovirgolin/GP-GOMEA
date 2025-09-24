/*
 


 */

/* 
 * File:   GOMEATreeInitializer.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 6:02 PM
 */

#ifndef GOMEATREEINITIALIZER_H
#define GOMEATREEINITIALIZER_H

#include "GPGOMEA/Variation/TreeInitializer.h"

#include <armadillo>

class GOMEATreeInitializer : public TreeInitializer {
public:

    GOMEATreeInitializer(TreeInitType tit) : TreeInitializer(tit) {
    };

    Node* InitializeRandomTree(TreeInitShape init_shape, size_t max_height, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) const override;

    Node * GenerateTreeFull(size_t max_height_left, int actual_height_left, size_t max_arity, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) const;
    Node * GenerateTreeGrow(size_t max_height_left, int actual_height_left, int cur_height, size_t max_arity, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) const;
    Node * GenerateTreeHeuristic(size_t max_height_left, int actual_height_left, size_t actual_max_height, size_t max_arity, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) const;
    
private:


};

#endif /* GOMEATREEINITIALIZER_H */

