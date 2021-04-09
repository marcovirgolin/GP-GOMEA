/*
 


 */

/* 
 * File:   SemanticBackpropagator.cpp
 * Author: virgolin
 * 
 * Created on June 30, 2018, 6:20 PM
 */

#include "GPGOMEA/Semantics/SemanticBackpropagator.h"

using namespace std;
using namespace arma;

std::vector<arma::vec> SemanticBackpropagator::ComputeSemanticBackpropagation(const arma::mat& X, const arma::mat& Y, Node* n, bool use_caching, bool linear_scaling) {
    vector<arma::vec> desired_output;

    vector<Node *> ancestors;
    ancestors.reserve(32);
    Node * a = n;
    while (a != NULL) {
        ancestors.push_back(a);
        a = a->parent;
    }

    // set the desired_output of the root
    a = ancestors[ancestors.size() - 1];
    desired_output.reserve(Y.n_elem);
    arma::vec y;

    // if linear scaling is enabled, gotta take that into account
    if (linear_scaling) {
        vec out = a->GetOutput(X, use_caching);
        pair<double_t, double_t> ab = Utils::ComputeLinearScalingTerms(out, Y);
        vec Yprime = (Y - ab.first) / ab.second;
        for (size_t i = 0; i < Y.n_elem; i++) {
            y = vec(1);
            y[0] = Yprime[i];
            desired_output.push_back(y);
        }
    } else {
        for (size_t i = 0; i < Y.n_elem; i++) {
            y = vec(1);
            y[0] = Y[i];
            desired_output.push_back(y);
        }
    }

    // set desired output traversing down 
    for (int i = ancestors.size() - 1; i > -1; i--) {
        a = ancestors[i];
        desired_output = a->GetDesiredOutput(desired_output, X, use_caching);
    }

    return desired_output;
}
