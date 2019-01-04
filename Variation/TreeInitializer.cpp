/*
 


 */

/* 
 * File:   TreeInitializer.cpp
 * Author: virgolin
 * 
 * Created on June 27, 2018, 4:30 PM
 */

#include "TreeInitializer.h"

using namespace std;
using namespace arma;

Node * TreeInitializer::InitializeRandomTree(TreeInitShape init_shape, size_t max_height, const std::vector<Operator *> & functions, const std::vector<Operator *> & terminals) const {

    size_t height = max_height;
    if (tree_init_type == TreeInitType::TreeInitRHH)
        height = randu() * (max_height + 1 - 2) + 2;
    
    if (init_shape == TreeInitFULL) {
        return GenerateTreeFull(height, functions, terminals);
    } else if (init_shape == TreeInitGROW) {
        return GenerateTreeGrow(height, 0, functions, terminals);
    }

    throw std::runtime_error("TreeInitializer::InitializeRandomTree invalid tree initialization type");
}

Node* TreeInitializer::GenerateTreeFull(size_t height_left, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) const {
    Node * curr;
    double_t rnd = arma::randu();
    size_t index;

    if (height_left > 0) {
        index = rnd * functions.size();
        curr = new Node(functions[index]->Clone());
    } else {
        index = rnd * terminals.size();
        curr = new Node(terminals[index]->Clone());
    }

    for (size_t i = 0; i < curr->GetArity(); i++) {
        curr->AppendChild(GenerateTreeFull(height_left - 1, functions, terminals));
    }

    return curr;
}

Node* TreeInitializer::GenerateTreeGrow(size_t height_left, size_t cur_height, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) const {

    Node * curr = NULL;
    double_t rnd = arma::randu();

    size_t index;
    if (height_left > 0) {
        if (rnd >= 0.5) { // || height_left > 2 ) {
            rnd = arma::randu();
            index = rnd * functions.size();
            curr = new Node(functions[index]->Clone());
        } else {
            rnd = arma::randu();
            index = rnd * terminals.size();
            curr = new Node((terminals[index])->Clone());
        }

    } else {
        rnd = arma::randu();
        index = rnd * terminals.size();
        curr = new Node((terminals[index])->Clone());
    }

    for (size_t i = 0; i < curr->GetArity(); i++) {
        curr->AppendChild(GenerateTreeGrow(height_left - 1, cur_height + 1, functions, terminals));
    }

    return curr;
}
