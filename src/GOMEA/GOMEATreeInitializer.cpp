/*
 


 */

/* 
 * File:   GOMEATreeInitializer.cpp
 * Author: virgolin
 * 
 * Created on June 27, 2018, 6:02 PM
 */

#include "GPGOMEA/GOMEA/GOMEATreeInitializer.h"

using namespace std;
using namespace arma;

Node* GOMEATreeInitializer::InitializeRandomTree(TreeInitShape init_shape, size_t max_height, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) const {
    
    size_t max_arity = 0;
    for (Operator * op : functions) {
        if (op->arity > max_arity)
            max_arity = op->arity;
    }

    int height = max_height;
    if (tree_init_type == TreeInitType::TreeInitRHH) {
        height = randu() * max_height;
    }

    Node * n;

    if (init_shape == TreeInitFULL) {
        n = GenerateTreeFull(max_height, height, max_arity, functions, terminals);
    } else if (init_shape == TreeInitGROW) {
        n = GenerateTreeGrow(max_height, height, -1, max_arity, functions, terminals);
    } else {
        throw std::runtime_error("GOMEATreeInitializer::InitializeRandomTree invalid tree initialization type");
    }

    return n;
}

Node* GOMEATreeInitializer::GenerateTreeGrow(size_t max_height_left, int actual_height_left, int cur_height, size_t max_arity, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) const {
    Node * curr = NULL;
    double_t rnd = arma::randu();

    size_t index;
    if (max_height_left > 0) {
        if (actual_height_left > 0 && rnd < 0.5) {
            //        if (actual_height_left > 0 && (rnd >= 0.5 || cur_height < 1)) {
            rnd = arma::randu();
            index = rnd * functions.size();
            curr = new Node(functions[index]->Clone());
        } else {
            rnd = arma::randu();
            index = rnd * terminals.size();
            curr = new Node((terminals[index])->Clone());
        }

        for (size_t i = 0; i < max_arity; i++) {
            curr->AppendChild(GenerateTreeGrow(max_height_left - 1, actual_height_left - 1, cur_height + 1, max_arity, functions, terminals));
        }

    } else {
        rnd = arma::randu();
        index = rnd * terminals.size();
        curr = new Node((terminals[index])->Clone());
    }

    return curr;
}

Node * GOMEATreeInitializer::GenerateTreeFull(size_t max_height_left, int actual_height_left, size_t max_arity, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) const {
    Node * curr;
    double_t rnd = arma::randu();
    size_t index;

    if (max_height_left > 0) {
        if (actual_height_left > 0) {
            index = rnd * functions.size();
            curr = new Node(functions[index]->Clone());
        } else {
            index = rnd * terminals.size();
            curr = new Node(terminals[index]->Clone());
        }

        for (size_t i = 0; i < max_arity; i++) {
            curr->AppendChild(GenerateTreeFull(max_height_left - 1, actual_height_left - 1, max_arity, functions, terminals));
        }

    } else {
        index = rnd * terminals.size();
        curr = new Node(terminals[index]->Clone());
    }


    return curr;
}
