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

    bool use_normalization = semlib.normalize_outputs;

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
        // do algebraic simplifications
        bool simplification_done = false;

        if (use_normalization) {
            // potential simplifications
            if (false && parent_o->GetValue().compare("*") == 0) {
                Node * slope_n = NULL;
                Node * intercept_n = NULL;
                double_t c;
                Node * sibling_n;
                vector<Node*> siblings = parent_o->children;

                if (siblings[0] == chosen_o) {
                    sibling_n = siblings[1];
                } else {
                    sibling_n = siblings[0];
                }

                if (sibling_n->op->type == OperatorType::opTerminal &&
                        sibling_n->op->term_type == OperatorTerminalType::opTermConstant) {
                    // sibling is a constant
                    c = ((OpRegrConstant*) sibling_n->op)->GetConstant();

                    Node * times_n = NULL;
                    if (branch->GetValue().compare("+") == 0) {
                        // look for the intercept
                        if (branch->children[0]->GetValue().compare("*") == 0) {
                            times_n = branch->children[0];
                            intercept_n = branch->children[1];
                        } else {
                            times_n = branch->children[1];
                            intercept_n = branch->children[0];
                        }
                        if (!(intercept_n->op->type == OperatorType::opTerminal && intercept_n->op->term_type == OperatorTerminalType::opTermConstant)) {
                            intercept_n = NULL;
                        }
                    }

                    if (intercept_n != NULL) {
                        // look for the slope
                        if (Utils::IsNumber(times_n->children[0]->GetValue())) {
                            slope_n = times_n->children[0];
                        } else if (Utils::IsNumber(times_n->children[1]->GetValue())) {
                            slope_n = branch->children[1];
                        }

                        if (!(slope_n->op->type == OperatorType::opTerminal && slope_n->op->term_type == OperatorTerminalType::opTermConstant)) {
                            slope_n = NULL;
                        }
                    }
                }

                if (slope_n && intercept_n) {

                    double_t new_intercept = c * ((OpRegrConstant*) intercept_n->op)->GetConstant();
                    double_t new_slope = c * ((OpRegrConstant*) slope_n->op)->GetConstant();

                    if (!(isinf(new_intercept) || isnan(new_intercept) || isinf(new_slope) || isnan(new_slope))) {
                        ((OpRegrConstant*) intercept_n->op)->SetConstant(new_intercept);
                        ((OpRegrConstant*) slope_n->op)->SetConstant(new_slope);

                        if (use_caching) {
                            slope_n->ClearCachedOutput(true);
                            intercept_n->ClearCachedOutput(false);
                        }

                        Node * pp = parent_o->parent;
                        chosen_o = parent_o;
                        parent_o = pp;
                        simplification_done = true;
                    }
                }
            } else if (false && parent_o->GetValue().compare("+") == 0) {
                Node * intercept_n = NULL;
                double_t c;
                Node * sibling_n;
                vector<Node*> siblings = parent_o->children;

                if (siblings[0] == chosen_o) {
                    sibling_n = siblings[1];
                } else {
                    sibling_n = siblings[0];
                }

                if (sibling_n->op->type == OperatorType::opTerminal &&
                        sibling_n->op->term_type == OperatorTerminalType::opTermConstant) {
                    // sibling is a constant
                    c = ((OpRegrConstant*) sibling_n->op)->GetConstant();

                    if (branch->GetValue().compare("+") == 0) {
                        // look for the intercept
                        if (branch->children[0]->op->type == OperatorType::opTerminal && intercept_n->op->term_type == OperatorTerminalType::opTermConstant == 0) {
                            intercept_n = branch->children[0];
                        } else if (branch->children[1]->op->type == OperatorType::opTerminal && intercept_n->op->term_type == OperatorTerminalType::opTermConstant == 0) {
                            intercept_n = branch->children[1];
                        }
                    }

                    if (intercept_n) {

                        double_t new_intercept = c + ((OpRegrConstant*) intercept_n->op)->GetConstant();

                        if (!(isinf(new_intercept) || isnan(new_intercept))) {
                            ((OpRegrConstant*) intercept_n->op)->SetConstant(new_intercept);

                            if (use_caching) {
                                intercept_n->ClearCachedOutput(false);
                            }

                            Node * pp = parent_o->parent;
                            chosen_o = parent_o;
                            parent_o = pp;
                            simplification_done = true;
                        }
                    }
                }
            }
        }

        if (!simplification_done || (simplification_done && parent_o)) {
            auto it1 = parent_o->DetachChild(chosen_o);
            parent_o->InsertChild(branch, it1);
            chosen_o->ClearSubtree();
        } else {
            o->ClearSubtree();
            o = branch;
        }

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
                    if (!isnan(vals[i])) {
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
            if (isinf(desired_vals_mean) || isnan(desired_vals_mean)) {
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
        for(size_t i = 0; i < desired_output.size(); i++) {
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
                if (isnan(v))
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

Node * SubtreeVariator::GradientDescent(Node& parent, Fitness & fitness, bool unif_depth_var, bool use_caching, bool use_linear_scaling, double_t learning_rate, double_t min_step_size, size_t iterations, bool only_on_constants) {

    Node * o = parent.CloneSubtree();
    double_t curr_fit = parent.cached_fitness;

    vector<Node*> nodes = o->GetSubtreeNodes(true, SubtreeParseType::SubtreeParsePREORDER); // we will use 0 to reference the root

    if (use_linear_scaling && nodes.size() == 1) // pointless to do gradient descent on the root if linear scaling is enabled
        return o;

    vector<Node*> chosen_nodes;
    chosen_nodes.reserve(nodes.size());
    if (only_on_constants) {
        for (Node * n : nodes) {
            // exclude root if linear scaling is enabled
            if (use_linear_scaling && n == nodes[0]) {
                continue;
            }
            if (n->op->type == OperatorType::opTerminal
                    && n->op->term_type == OperatorTerminalType::opTermConstant)
                chosen_nodes.push_back(n);
        }
    } else {
        size_t exclude_root_if_linear_scaling_factor = 0;
        if (use_linear_scaling)
            exclude_root_if_linear_scaling_factor = 1;

        size_t num_nodes = 1; //(randu() * (nodes.size() - exclude_root_if_linear_scaling_factor)) / 2;
        vector<Node*> candidate_nodes = nodes;

        set<Node *> chosen_nodes_set;

        while (chosen_nodes_set.size() < num_nodes) {

            if (unif_depth_var) {
                candidate_nodes = GetNodesAtUniformRandomDepth(o, nodes);
                // exclude root if linear scaling is enabled
                if (use_linear_scaling && candidate_nodes.size() == 1 && !candidate_nodes[0]->parent)
                    continue;
            }

            size_t idx = randu() * candidate_nodes.size();
            chosen_nodes_set.insert(candidate_nodes[idx]);
        }
        chosen_nodes = vector<Node*>(chosen_nodes_set.begin(), chosen_nodes_set.end());

        // create symbolic infrastructure to perform gradient descent
        for (size_t j = 0; j < chosen_nodes.size(); j++) {
            Node * n = chosen_nodes[j];
            if (n->op->type == OperatorType::opTerminal
                    && n->op->term_type == OperatorTerminalType::opTermConstant)
                continue;

            // The chosen node is not a constant. Check if sibling is, and the parent is plus or minus
            bool infrastracture_already_present = false;
            Node * p = n->parent;
            if (p) {
                bool p_is_plus = p->op->name.compare("+") == 0;
                bool p_is_minus = !p_is_plus && p->op->name.compare("-") == 0;
                // look for a constant sibling
                if (p_is_plus || p_is_minus) {
                    for (Node * s : p->children) {
                        if (s != n && s->op->type == OperatorType::opTerminal
                                && s->op->term_type == OperatorTerminalType::opTermConstant) {
                            // here's the sibling we can update with gradient descent! Let's also set the parent to a + for convenience tough
                            infrastracture_already_present = true;
                            chosen_nodes[j] = s;
                            if (p_is_minus) {
                                delete p->op;
                                p->op = new OpPlus();
                            }
                            break;
                        }
                    }
                }
            }
            // create the infrastructure if not already present
            if (!infrastracture_already_present) {
                Node * plus = new Node(new OpPlus());
                Node * cons = new Node(new OpRegrConstant(0.0));

                if (p) {
                    auto it = p->DetachChild(n);
                    p->InsertChild(plus, it);
                } else {
                    o = plus;
                }
                if (randu() < 0.5) {
                    plus->AppendChild(n);
                    plus->AppendChild(cons);
                } else {
                    plus->AppendChild(cons);
                    plus->AppendChild(n);
                }

                chosen_nodes[j] = cons;
            }

            assert(chosen_nodes[j]->op->term_type == OperatorTerminalType::opTermConstant);
        }
    }

    if (!chosen_nodes.empty()) {

        // adam momentum 
        double_t beta_1 = 0.9;
        double_t beta_2 = 0.999;
        double_t eps = 1e-8;

        vector<double_t> m_ts(chosen_nodes.size(), 0.0);
        vector<double_t> v_ts(chosen_nodes.size(), 0.0);

        for (size_t i = 0; i < iterations; i++) {

            vec output = o->GetOutput(fitness.TrainX, use_caching);
            vector<double_t> gradients;
            gradients.reserve(chosen_nodes.size());
            vector<double_t> sq_gradients;
            sq_gradients.reserve(chosen_nodes.size());

            double_t scaling_factor = 1.0;
            if (use_linear_scaling) {
                pair<double_t, double_t> linear_scaling_terms = Utils::ComputeLinearScalingTerms(output, fitness.TrainY, &fitness.trainY_mean, &fitness.var_comp_trainY);
                scaling_factor = linear_scaling_terms.second;
            }

            for (Node * c : chosen_nodes) {
                double_t gradient = -2 * arma::mean((fitness.TrainY - scaling_factor * output) % o->GetDerivative(c, fitness.TrainX, use_caching));
                if (isnan(gradient))
                    break;
                gradients.push_back(gradient);
                sq_gradients.push_back(gradient * gradient);
            }

            // update constants
            vector<double_t> previous_constant_values;
            previous_constant_values.reserve(chosen_nodes.size());
            size_t number_of_updates = 0;
            for (size_t j = 0; j < chosen_nodes.size(); j++) {

                // update momentum
                m_ts[j] = beta_1 * m_ts[j] + (1 - beta_1) * gradients[j];
                v_ts[j] = beta_2 * v_ts[j] + (1 - beta_2) * sq_gradients[j];

                double_t step = learning_rate * gradients[j];
                //double_t step = learning_rate / (sqrt(v_ts[j] / (1 - pow(beta_2, i))) + eps) * m_ts[j] / (1 - pow(beta_1, i));
                double_t curr = ((OpRegrConstant *) chosen_nodes[j]->op)->GetConstant();
                previous_constant_values.push_back(curr);
                if (isnan(step) || step < min_step_size)
                    continue;
                number_of_updates++;

                //assert(!isnan(curr));
                double_t new_c = curr - step;
                ((OpRegrConstant *) chosen_nodes[j]->op)->SetConstant(new_c);
                if (use_caching)
                    chosen_nodes[j]->ClearCachedOutput(true);
            }

            if (number_of_updates > 0) {
                // recompute error
                double_t fit = fitness.ComputeFitness(o, use_caching);
                if (fit > curr_fit || isnan(fit) || isinf(fit)) {
                    for (size_t j = 0; j < chosen_nodes.size(); j++) {
                        ((OpRegrConstant *) chosen_nodes[j]->op)->SetConstant(previous_constant_values[j]);
                        if (use_caching)
                            chosen_nodes[j]->ClearCachedOutput(true);
                        o->cached_fitness = curr_fit;
                    }
                    break;
                } else {
                    curr_fit = fit;
                }
            }

        }

        // simplify tree by removing unused constants (i.e., +0.0)
        if (!only_on_constants) {
            for (size_t j = 0; j < chosen_nodes.size(); j++) {

                double_t const_val = ((OpRegrConstant *) chosen_nodes[j]->op)->GetConstant();
                Node * p = chosen_nodes[j]->parent;
                if (const_val == 0.0 && p && (p->GetValue().compare("+") == 0 || p->GetValue().compare("-") == 0)) {
                    Node * pp = p->parent;
                    Node * sibling;
                    for (Node * s : p->children) {
                        if (s != chosen_nodes[j]) {
                            sibling = s;
                            break;
                        }
                    }
                    p->DetachChild(sibling);
                    if (pp) {
                        auto it = pp->DetachChild(p);
                        pp->InsertChild(sibling, it);
                    } else {
                        o = sibling;
                    }

                    delete chosen_nodes[j];
                    delete p;
                }
            }
        }
    }

    return o;
}
