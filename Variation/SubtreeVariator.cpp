/*
 


 */

/* 
 * File:   SubtreeVariator.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 3:12 PM
 */

#include "SubtreeVariator.h"

using namespace std;
using namespace arma;

std::pair<Node*, Node*> SubtreeVariator::SubtreeCrossover(const Node & p1, const Node & p2, bool unif_depth_var, bool use_caching) {

    Node * o1 = p1.CloneSubtree();
    Node * o2 = p2.CloneSubtree();

    vector<Node*> nodes_o1 = o1->GetSubtreeNodes(true);
    vector<Node*> nodes_o2 = o2->GetSubtreeNodes(true);

    if (unif_depth_var) {
        nodes_o1 = GetNodesAtUniformRandomDepth(o1, nodes_o1);
        nodes_o2 = GetNodesAtUniformRandomDepth(o2, nodes_o2);
    }

    // pick nodes to swap
    Node * chosen_o1 = nodes_o1 [randu() * nodes_o1.size()];
    Node * chosen_o2 = nodes_o2 [randu() * nodes_o2.size()];

    // perform swap
    Node * parent_o1 = chosen_o1->parent;
    Node * parent_o2 = chosen_o2->parent;
    if (!parent_o1 && !parent_o2) {
        // swapping roots preserves the entire trees.
        // so we can just do nothing. 
    } else if (!parent_o1 && parent_o2) {
        auto it2 = parent_o2->DetachChild(chosen_o2);
        parent_o2->InsertChild(chosen_o1, it2);
        o1 = chosen_o2;
    } else if (parent_o1 && !parent_o2) {
        auto it1 = parent_o1->DetachChild(chosen_o1);
        parent_o1->InsertChild(chosen_o2, it1);
        o2 = chosen_o1;
    } else {
        auto it1 = parent_o1->DetachChild(chosen_o1);
        auto it2 = parent_o2->DetachChild(chosen_o2);
        parent_o1->InsertChild(chosen_o2, it1);
        parent_o2->InsertChild(chosen_o1, it2);
    }

    // delete cached outputs
    if (use_caching) {
        chosen_o1->ClearCachedOutput(true);
        chosen_o2->ClearCachedOutput(true);
    }

    return make_pair(o1, o2);
}

Node* SubtreeVariator::SubtreeMutation(const Node & p, const TreeInitializer & tree_initializer, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals, size_t initial_max_tree_height, bool unif_depth_var, bool use_caching) {

    Node * branch = tree_initializer.InitializeRandomTree(TreeInitShape::TreeInitGROW, initial_max_tree_height, functions, terminals);

    Node * o = p.CloneSubtree();

    vector<Node*> nodes_o = o->GetSubtreeNodes(true);

    if (unif_depth_var) {
        // pick nodes candidates for swapping by first choosing a random depth
        nodes_o = GetNodesAtUniformRandomDepth(o, nodes_o);
    }

    // pick nodes to swap
    Node * chosen_o = nodes_o[randu() * nodes_o.size()];

    // perform swap
    Node * parent_o = chosen_o->parent;
    if (parent_o) {
        auto it1 = parent_o->DetachChild(chosen_o);
        parent_o->InsertChild(branch, it1);
        chosen_o->ClearSubtree();
    } else {
        o->ClearSubtree();
        o = branch;
    }

    // delete cached outputs
    if (use_caching) {
        branch->ClearCachedOutput(true);
    }

    return o;
}

Node* SubtreeVariator::TopMutation(const Node& p, const TreeInitializer& tree_initializer, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals, size_t initial_max_tree_height) {

    Node * branch = tree_initializer.InitializeRandomTree(TreeInitShape::TreeInitGROW, initial_max_tree_height, functions, terminals);

    Node * randomleaf, * leafparent;

    vector<Node*> branchnodes = branch->GetSubtreeNodes(true);
    vector<Node*> candidates;
    candidates.reserve(branchnodes.size() / 2 + 1);
    for (Node * n : branchnodes) {
        if (n->children.empty()) {
            candidates.push_back(n);
        }
    }
    randomleaf = candidates[randu() * candidates.size()];
    leafparent = randomleaf->parent;

    if (!leafparent)
        return randomleaf;

    Node * o = p.CloneSubtree();
    auto it = leafparent->DetachChild(randomleaf);
    leafparent->InsertChild(o, it);

    return branch;
}

std::vector<Node*> SubtreeVariator::GetNodesAtUniformRandomDepth(Node* o, vector<Node*>& nodes_o) {

    size_t max_d = o->GetHeight();
    size_t d = randu() * (max_d + 1);

    vector<Node*> candidates_o;
    candidates_o.reserve(nodes_o.size() / 2 + 1);

    for (Node * n : nodes_o) {
        if (n->GetDepth() == d)
            candidates_o.push_back(n);
    }

    return candidates_o;
}

Node* SubtreeVariator::SubtreeRDO(const Node& p, int max_height, const arma::mat & X, const arma::vec & Y, SemanticLibrary & semlib, bool unif_depth_var, bool use_caching, bool is_linear_scaling_enabled) {

    Node * o = p.CloneSubtree();

    Node * chosen_o;
    vector<Node*> nodes_o = o->GetSubtreeNodes(true);

    if (unif_depth_var)
        // pick nodes candidates for swapping by first choosing a random depth
        nodes_o = GetNodesAtUniformRandomDepth(o, nodes_o);

    // pick nodes to change
    chosen_o = nodes_o[randu() * nodes_o.size()];

    vector<vec> desired_output = SemanticBackpropagator::ComputeSemanticBackpropagation(X, Y, chosen_o, use_caching, is_linear_scaling_enabled);
    if (desired_output.size() == 1) { // impossibility
        return o;
    }

    size_t current_depth = chosen_o->GetDepth();
    size_t max_h = max_height - current_depth;

    Node * branch = FindSubtreeForSemanticBackpropagationReplacement(desired_output, semlib, max_h);
    if (!branch)
        return o;

    // perform swap
    Node * parent_o = chosen_o->parent;
    if (parent_o) {

        auto it1 = parent_o->DetachChild(chosen_o);
        parent_o->InsertChild(branch, it1);
        chosen_o->ClearSubtree();

    } else {
        // replacing the root
        o->ClearSubtree();
        o = branch;
    }

    // delete cached outputs
    if (use_caching) {
        branch->ClearCachedOutput(true);
    }

    return o;
}

std::pair<Node*, Node*> SubtreeVariator::SubtreeAGX(Node& p1, Node& p2, int max_height, const arma::mat & X, SemanticLibrary& semlib, bool unif_depth_var, bool use_caching, bool is_linear_scaling_enabled) {

    vec Y1 = p2.GetOutput(X, use_caching);
    vec Y2 = p1.GetOutput(X, use_caching);

    vec MidPoint = (Y1 + Y2) / 2;

    Node * o1 = SubtreeRDO(p1, max_height, X, MidPoint, semlib, unif_depth_var, use_caching, is_linear_scaling_enabled);
    Node * o2 = SubtreeRDO(p2, max_height, X, MidPoint, semlib, unif_depth_var, use_caching, is_linear_scaling_enabled);

    return make_pair(o1, o2);
}

Node * SubtreeVariator::FindSubtreeForSemanticBackpropagationReplacement(std::vector<vec> & desired_output, SemanticLibrary & semlib, size_t max_h) {

    double_t desired_vals_mean = 0.0;

    vec vals;
    vec vals_wo_mean;
    vector<double_t> vals_no_nans;

    bool use_normalization = semlib.normalize_outputs;
    bool search_for_normalized_tree = use_normalization;

    bool use_lib_linear_parsing = semlib.linear_parse_library;

    if (search_for_normalized_tree) {
        search_for_normalized_tree = max_h > 1;
    } else if (search_for_normalized_tree) {
        search_for_normalized_tree = max_h > 0;
    }

    if (search_for_normalized_tree) {

        vals = vec(desired_output.size());
        vals_no_nans.reserve(vals.n_elem);

        for (size_t i = 0; i < desired_output.size(); i++) {
            vals[i] = arma::datum::inf;
            for (size_t j = 0; j < desired_output[i].n_elem; j++) {
                if (!isinf(desired_output[i][j])) {
                    vals[i] = desired_output[i][j];
                    if (!std::isnan(vals[i])) {
                        vals_no_nans.push_back(vals[i]);
                        desired_vals_mean += vals[i];
                    }
                    break;
                }
            }
        }

        if (!vals_no_nans.empty())
            desired_vals_mean /= vals_no_nans.size();
        // else it remained 0.0

        vals_wo_mean = (vals - desired_vals_mean);

        if (vals_no_nans.empty()) {
            // anything is fine actually, so let's set up a random query
            vals_wo_mean = vec(vals.n_elem);
            for (size_t i = 0; i < vals.n_elem; i++) {
                vals_wo_mean[i] = arma::randn(); //random normal cause mean 0 std 1 like normalized trees
            }
            // could recompute desired_vals_mean, but that should be close to 0 anyway (current value) due to normal distributed samples above

        } else if (!use_lib_linear_parsing) { // compute the mean, make sure its ok
            if (isinf(desired_vals_mean) || std::isnan(desired_vals_mean)) {
                search_for_normalized_tree = false;
            }
        }

    }

    auto best_n_dist = make_pair((Node*) NULL, arma::datum::inf);
    pair<double_t, double_t> normalization_intercept_slope = make_pair(arma::datum::nan, arma::datum::nan);

    if (search_for_normalized_tree) {
        max_h -= 1;

        if (use_lib_linear_parsing) {
            max_h -= 1;
            best_n_dist = semlib.GetClosestSubtree(vals, max_h, &normalization_intercept_slope);
        } else {
            best_n_dist = semlib.GetClosestSubtree(vals_wo_mean, max_h);
            auto best_mean_std = semlib.normalization_stored_solutions_mean_std[best_n_dist.first];
            normalization_intercept_slope.first = desired_vals_mean - best_mean_std.first;
        }

    } else if (!use_normalization) {
        // perform normal search (trees in lib are not normalized)
        best_n_dist = semlib.GetClosestSubtree(desired_output, max_h);
    }

    pair<double_t, double_t> const_cdist;
    if (use_normalization) {
        vec constant_mean_vector(desired_output.size());
        for (size_t i = 0; i < desired_output.size(); i++) {
            constant_mean_vector[i] = desired_vals_mean;
        }
        double_t distconst = Utils::ComputeDistanceWithDontCares(desired_output, constant_mean_vector);
        const_cdist = make_pair(desired_vals_mean, distconst);
    } else {
        const_cdist = GenerateConstantFromDesiredOutput(desired_output, best_n_dist.second);
    }
    Node * result;

    if (isinf(const_cdist.second) && isinf(best_n_dist.second)) {
        return NULL;
    }

    if (const_cdist.second < best_n_dist.second) {

        result = new Node(new OpRegrConstant(const_cdist.first));

    } else {

        result = best_n_dist.first->CloneSubtree();

        // we searched for a normalized tree, gotta add the constants to scale and transpose
        if (search_for_normalized_tree) {

            Node * plus, * times;
            times = result;
            if (use_lib_linear_parsing && normalization_intercept_slope.second != 1.0) {
                // add scaling node
                Node * scaling_n = new Node(new OpRegrConstant(normalization_intercept_slope.second));

                times = new Node(new OpTimes());
                // add stuff in random order
                if (arma::randu() < 0.5) {
                    times->AppendChild(scaling_n);
                    times->AppendChild(result);
                } else {
                    times->AppendChild(result);
                    times->AppendChild(scaling_n);
                }
            }

            plus = times;

            if (normalization_intercept_slope.first != 0.0) {
                // add intercept node 
                Node * intercept_n = new Node(new OpRegrConstant(normalization_intercept_slope.first));
                plus = new Node(new OpPlus());

                // add stuff in random order
                if (arma::randu() < 0.5) {
                    plus->AppendChild(intercept_n);
                    plus->AppendChild(times);
                } else {
                    plus->AppendChild(times);
                    plus->AppendChild(intercept_n);
                }
            }

            result = plus;
        }
    }

    return result;
}

std::pair<double_t, double_t> SubtreeVariator::GenerateConstantFromDesiredOutput(std::vector<arma::vec>& desired_output, double_t thresh_dist) {

    double_t constant;
    double_t min_c_dist = datum::inf;

    set<double_t> candidate_constants;
    for (vec & x : desired_output) {
        for (size_t i = 0; i < x.n_elem; i++) {
            candidate_constants.insert(x[i]);
        }
    }

    for (double_t c : candidate_constants) {

        double_t dist = 0;

        for (vec & x : desired_output) {
            double_t min_d_within_x = datum::inf;
            for (double_t v : x) {
                if (std::isnan(v))
                    continue;
                double_t d = (v - c)*(v - c);
                if (d < min_d_within_x) {
                    min_d_within_x = d;
                }
            }
            dist += min_d_within_x;

            if (dist > thresh_dist) {
                dist = datum::inf;
                break;
            }
        }

        //dist = dist;

        if (dist < min_c_dist) {
            min_c_dist = dist;
            constant = c;
        }
    }

    return make_pair(constant, min_c_dist);
}
