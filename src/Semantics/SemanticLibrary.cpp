/*
 


 */

/* 
 * File:   SemanticLibrary.cpp
 * Author: virgolin
 * 
 * Created on July 5, 2018, 9:11 PM
 */

#include "GPGOMEA/Semantics/SemanticLibrary.h"

using namespace std;
using namespace arma;

SemanticLibrary::SemanticLibrary(bool normalize_outputs, bool linear_parse_library) {
    this->normalize_outputs = normalize_outputs;
    this->linear_parse_library = linear_parse_library;
}

SemanticLibrary::~SemanticLibrary() {
    for (auto it = stored_solutions.begin(); it != stored_solutions.end(); it++)
        it->second.first->ClearSubtree();
}

void SemanticLibrary::GeneratePopulationLibrary(size_t max_height, size_t max_library_size, const std::vector<Node*>& population, Fitness& fitness, bool caching) {

    libraries_kdtrees.clear();
    libraries_vectors.clear();

    already_stored_semantics.clear();
    for (auto it = stored_solutions.begin(); it != stored_solutions.end(); it++)
        it->second.first->ClearSubtree();
    stored_solutions.clear();

    for (Node * p : population) {

        vector<Node*> nodes = p->GetSubtreeNodes(true);
        for (Node * n : nodes) {

            if (n->GetHeight(false) > max_height)
                continue;

            Node * m = n->CloneSubtree();

            bool success = AddToLibrary(m, fitness, caching);

            if (success && stored_solutions.size() >= max_library_size)
                break;

            if (!success)
                m->ClearSubtree();
        }

        if (stored_solutions.size() >= max_library_size)
            break;
    }

    cout << " ~ generated semantic library (tot. size " << stored_solutions.size() << ": ";
    if (linear_parse_library) {
        for (auto it = stored_solutions.begin(); it != stored_solutions.end(); it++) {
            size_t h = it->second.first->GetHeight(true);
            libraries_vectors[h].push_back(it->second);
        }
    } else {
        for (auto it = libraries_kdtrees.begin(); it != libraries_kdtrees.end(); it++) {
            it->second.build();
            cout << "h=" << it->first << ":s=" << it->second.getPointsNumber() << " ";
        }
    }
    cout << ")" << endl;

}

void SemanticLibrary::GenerateRandomLibrary(size_t max_height, size_t max_library_size, Fitness & fitness, const std::vector<Operator*> & functions, const std::vector<Operator*> & terminals, const TreeInitializer & tree_init, bool caching) {

    libraries_kdtrees.clear();
    libraries_vectors.clear();
    already_stored_semantics.clear();
    for (auto it = stored_solutions.begin(); it != stored_solutions.end(); it++) {
        it->second.first->ClearSubtree();
    }
    stored_solutions.clear();

    // strip off constants from terminals
    /*vector<Operator*> terminals_wo_const;
    terminals_wo_const.reserve(terminals.size());
    for (Operator * op : terminals) {
        if (dynamic_cast<OpRegrConstant*> (op))
            continue;
        terminals_wo_const.push_back(op);
    }*/

    Node * n;
    TreeInitShape tis;
    size_t tries = 0;

    while (stored_solutions.size() < max_library_size && tries < max_tries) {

        size_t curr_max_h = randu() * (max_height - 2 + 1) + 2;

        if (randu() > 0.5)
            tis = TreeInitShape::TreeInitFULL;
        else
            tis = TreeInitShape::TreeInitGROW;

        n = tree_init.InitializeRandomTree(tis, curr_max_h, functions, terminals);

        bool success = AddToLibrary(n, fitness, caching);

        if (!success) {
            n->ClearSubtree();
        }

        tries++;
    }

    cout << " ~ generated semantic library (tot. size " << stored_solutions.size() << ": ";
    if (linear_parse_library) {
        for (auto it = stored_solutions.begin(); it != stored_solutions.end(); it++) {
            size_t h = it->second.first->GetHeight(true);
            libraries_vectors[h].push_back(it->second);
        }

        for (auto it = libraries_vectors.begin(); it != libraries_vectors.end(); it++) {
            cout << "h=" << it->first << ":s=" << it->second.size() << " ";
        }
    } else {
        for (auto it = libraries_kdtrees.begin(); it != libraries_kdtrees.end(); it++) {
            it->second.build();
            cout << "h=" << it->first << ":s=" << it->second.getPointsNumber() << " ";
        }
    }
    cout << ")" << endl;
}

bool SemanticLibrary::AddToLibrary(Node* n, Fitness& fitness, bool caching) {

    vec out = n->GetOutput(fitness.TrainX, caching);
    pair<double_t, double_t> mean_std;
    if (normalize_outputs) {
        mean_std = Utils::ComputeMeanStdEfficiently(out);
        if (std::isinf(mean_std.first) || std::isnan(mean_std.first) ||
                mean_std.second == 0 || std::isinf(mean_std.second) 
                || std::isnan(mean_std.second))
            return false;
        if (!linear_parse_library)
            out = out - mean_std.first;
    }

    // check out it is not a constant
    bool is_c = true;
    if (normalize_outputs) {
        is_c = mean_std.second == 0.0;
    } else {
        for (size_t i = 1; i < out.n_elem; i++) {
            if (out[0] != out[i]) {
                is_c = false;
                break;
            }
        }
    }
    if (is_c) {
        return false;
    }

    // try to insert in the library
    size_t hash = Utils::HashArmaVec(out);

    bool add = true;

    int h = -1;

    if (already_stored_semantics.find(hash) != already_stored_semantics.end()) {
        // see if this is smaller, in that case replace the existing one
        Node * other = stored_solutions[hash].first;
        size_t size = n->GetSubtreeNodes(true).size();
        size_t o_size = other->GetSubtreeNodes(true).size();
        size_t o_height = other->GetHeight(true);
        h = n->GetHeight(true);
        if (o_size > size) {
            other->ClearSubtree();
            if (o_height != h) {
                if (!linear_parse_library)
                    libraries_kdtrees[o_height].deletePoint(out);
            }

        } else {
            add = false;
        }
    }

    if (add) {
        already_stored_semantics.insert(hash);
        stored_solutions[hash] = make_pair(n, out);

        if (h == -1)
            h = n->GetHeight(false);

        if (!linear_parse_library)
            libraries_kdtrees[h].addPoint(out);

        if (normalize_outputs) {
            normalization_stored_solutions_mean_std[n] = make_pair(mean_std.first, mean_std.second);
        }
    }

    return add;
}

std::pair< Node *, double_t> SemanticLibrary::GetClosestSubtree(const arma::vec& x, size_t max_height, std::pair<double_t, double_t> * linear_scaling_coeffs, bool avoid_zero) {

    std::vector<vec> xx;
    xx.reserve(x.n_elem);

    vec v;
    for (size_t i = 0; i < x.n_elem; i++) {
        v = vec(1);
        v[0] = x[i];
        xx.push_back(v);
    }

    return GetClosestSubtree(xx, max_height, linear_scaling_coeffs, avoid_zero);

}

std::pair< Node *, double_t> SemanticLibrary::GetClosestSubtree(const std::vector< arma::vec > & x, size_t max_height, std::pair<double_t, double_t> * linear_scaling_coeffs, bool avoid_zero) {

    double_t dist;
    double_t best_min_dist = datum::inf;
    size_t best_idx;
    size_t best_h;
    vec best_out;
    vec query_vec;
    
    double_t mean_query;
    vec variation_components_query;

    Node * best_sol = NULL;
    bool linear_lib_parse_normalization_active = normalize_outputs && linear_scaling_coeffs;

    if (!linear_parse_library) {
        for (auto it = libraries_kdtrees.begin(); it != libraries_kdtrees.end(); it++) {
            if (it->first > max_height)
                continue;
            size_t res = libraries_kdtrees[it->first].nnSearch(x, &dist, avoid_zero);

            if (dist < best_min_dist) {
                best_min_dist = dist;
                best_idx = res;
                best_h = it->first;
            }
        }
    } else {

        if (linear_lib_parse_normalization_active) {

            query_vec = vec(x.size());
            for (size_t i = 0; i < x.size(); i++) {
                query_vec[i] = x[i][0];
            }

            mean_query = arma::mean(query_vec);
            variation_components_query = query_vec - mean_query;

            for (size_t h = 0; h < max_height; h++) {
                auto & v = libraries_vectors[h];
                for (size_t i = 0; i < v.size(); i++) {
                    dist = Utils::ComputeDistance(query_vec, v[i].second, true, &mean_query, &variation_components_query);

                    if (dist < best_min_dist) {
                        best_min_dist = dist;
                        best_sol = v[i].first;
                        if (linear_lib_parse_normalization_active)
                            best_out = v[i].second;
                    }
                }
            }
        } else {
            for (size_t h = 0; h < max_height; h++) {
                auto & v = libraries_vectors[h];
                for (size_t i = 0; i < v.size(); i++) {
                    double_t dist;
                    dist = Utils::ComputeDistanceWithDontCares(x, v[i].second);

                    if (dist < best_min_dist) {
                        best_min_dist = dist;
                        best_sol = v[i].first;
                        if (linear_lib_parse_normalization_active)
                            best_out = v[i].second;
                    }
                }
            }
        }
    }

    if (std::isinf(best_min_dist)) {
        return make_pair(best_sol, best_min_dist);
    }

    if (!linear_parse_library) {
        vec best_out = *libraries_kdtrees[best_h].getPoint(best_idx);
        size_t hash_best = Utils::HashArmaVec(best_out);
        best_sol = stored_solutions[hash_best].first;

    } else if (linear_lib_parse_normalization_active) {
        (*linear_scaling_coeffs) = Utils::ComputeLinearScalingTerms(best_out, query_vec, &mean_query, &variation_components_query);
    }

    return make_pair(best_sol, best_min_dist);
}

std::pair<Node*, double_t> SemanticLibrary::GetClosestSubtreeStochastic(const arma::vec& x, size_t max_height) {

    throw NotImplementedException("SemanticLibrary::GetClosestSubtreeStochastic not implemented"); //

}

std::pair<Node*, double_t> SemanticLibrary::GetClosestSubtreeStochastic(const std::vector<arma::vec> & x, size_t max_height) {

    throw NotImplementedException("SemanticLibrary::GetClosestSubtreeStochastic not implemented"); //

    /*vector<vec*> all_points;
    vector<int> all_res;
    vector<size_t> all_heights;

    size_t K = 3;

    for (auto it = libraries_kdtrees.begin(); it != libraries_kdtrees.end(); it++) {
        if (it->first > max_height)
            continue;
        vector<int> res = libraries_kdtrees[it->first].nnSearch(x, NULL);  // TODO: implement Knn with vector<vec> x
        vector<size_t> heights(K, it->first);
        all_res.insert(all_res.end(), res.begin(), res.end());
        all_heights.insert(all_heights.end(), heights.begin(), heights.end());

    }

    size_t winner_indx = randu() * all_points.size();
    

    vec best_out = *libraries_kdtrees[ all_heights[winner_indx] ].getPoint( all_res[winner_indx] );
    size_t hash_best = Utils::HashArmaVec( best_out );
    Node * best_sol = stored_solutions[hash_best].first;
    
    //double_t min_dist = arma::sum(arma::abs(best_out - x));
    
    return make_pair( best_sol, 1 );*/

}
