/*
 


 */

/* 
 * File:   TreeInitializer.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 4:30 PM
 */

#ifndef TREEINITIALIZER_H
#define TREEINITIALIZER_H

#include "../Genotype/Node.h"

#include <armadillo>
#include <iostream>

enum TreeInitType {
    TreeInitHH, TreeInitRHH
};

enum TreeInitShape {
    TreeInitFULL, TreeInitGROW
};

class TreeInitializer {
public:

    TreeInitializer(TreeInitType tit) {
        tree_init_type = tit;
    };

    virtual Node * InitializeRandomTree(TreeInitShape init_shape, size_t max_height, const std::vector<Operator *> & functions, const std::vector<Operator *> & terminals) const;

    Node * GenerateTreeGrow(size_t height_left, size_t cur_height, const std::vector<Operator *> & functions, const std::vector<Operator *> & terminals) const;
    Node * GenerateTreeFull(size_t height_left, const std::vector<Operator *> & functions, const std::vector<Operator *> & terminals) const;

    TreeInitType tree_init_type;

private:
};
#endif /* TREEINITIALIZER_H */

