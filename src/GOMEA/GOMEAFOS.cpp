/*
 


 */

/* 
 * File:   GOMEAFOS.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 5:40 PM
 */

#include <map>

#include "GPGOMEA/GOMEA/GOMEAFOS.h"


using namespace std;
using namespace arma;

std::vector<std::vector<size_t> > GOMEAFOS::GenerateFOS(const std::vector<Node*>& population, FOSType fos_type, bool fos_no_root_swap, arma::mat * MI_distribution_adjustments) {

    vector<vector < size_t>> FOS;

    size_t number_of_nodes = population[0]->GetSubtreeNodes(false).size();

    if (fos_type == FOSType::FOSLinkageTree)
        FOS = GenerateLinkageTreeFOS(number_of_nodes, population, MI_distribution_adjustments, fos_no_root_swap);
    else if (fos_type == FOSType::FOSRandomTree)
        FOS = GenerateRandomTreeFOS(number_of_nodes, fos_no_root_swap);
    else if (fos_type == FOSType::FOSUnivariate)
        FOS = GenerateUnivariateFOS(number_of_nodes, fos_no_root_swap);
    else
        throw std::runtime_error("GOMEAFOS::GenerateFOS unrecognized FOS type");

    return FOS;

}

std::vector<std::vector<size_t>> GOMEAFOS::BuildLinkageTreeFromSimilarityMatrix(size_t number_of_nodes, vector<vector<double_t>> &sim_matrix) {
    // code taken from Peter's

    vector<vector<size_t> > FOS;

    vector<size_t> random_order;
    random_order.reserve(number_of_nodes);
    for (size_t i = 0; i < number_of_nodes; i++)
        random_order.push_back(i);
    random_shuffle(random_order.begin(), random_order.end());

    vector<vector < int >> mpm(number_of_nodes, vector<int>(1));
    vector<int> mpm_number_of_indices(number_of_nodes);
    size_t mpm_length = number_of_nodes;

    for (size_t i = 0; i < number_of_nodes; i++) {
        mpm[i][0] = random_order[i];
        mpm_number_of_indices[i] = 1;
    }

    /* Initialize LT to the initial MPM */
    FOS.resize(number_of_nodes + number_of_nodes - 1);
    size_t FOSs_index = 0;
    for (size_t i = 0; i < mpm_length; i++) {
        FOS[FOSs_index] = vector<size_t>(mpm[i].begin(), mpm[i].end());
        FOSs_index++;
    }

    /* Initialize similarity matrix */
    vector<vector < double_t >> S_matrix(number_of_nodes, vector<double_t>(number_of_nodes));
    for (size_t i = 0; i < mpm_length; i++)
        for (size_t j = 0; j < mpm_length; j++)
            S_matrix[i][j] = sim_matrix[mpm[i][0]][mpm[j][0]];
    for (size_t i = 0; i < mpm_length; i++)
        S_matrix[i][i] = 0;

    vector<vector < int >> mpm_new;
    vector<int> NN_chain(number_of_nodes + 2, 0);
    size_t NN_chain_length = 0;
    short done = 0;

    while (!done) {
        if (NN_chain_length == 0) {
            NN_chain[NN_chain_length] = (int) randu() * mpm_length;
            NN_chain_length++;
        }

        while (NN_chain_length < 3) {
            NN_chain[NN_chain_length] = DetermineNearestNeighbour(NN_chain[NN_chain_length - 1], S_matrix, mpm_number_of_indices, mpm_length);
            NN_chain_length++;
        }

        while (NN_chain[NN_chain_length - 3] != NN_chain[NN_chain_length - 1]) {
            NN_chain[NN_chain_length] = DetermineNearestNeighbour(NN_chain[NN_chain_length - 1], S_matrix, mpm_number_of_indices, mpm_length);
            if (((S_matrix[NN_chain[NN_chain_length - 1]][NN_chain[NN_chain_length]] == S_matrix[NN_chain[NN_chain_length - 1]][NN_chain[NN_chain_length - 2]])) && (NN_chain[NN_chain_length] != NN_chain[NN_chain_length - 2]))
                NN_chain[NN_chain_length] = NN_chain[NN_chain_length - 2];
            NN_chain_length++;
            if (NN_chain_length > number_of_nodes)
                break;
        }
        int r0 = NN_chain[NN_chain_length - 2];
        int r1 = NN_chain[NN_chain_length - 1];
        int rswap;
        if (r0 > r1) {
            rswap = r0;
            r0 = r1;
            r1 = rswap;
        }
        NN_chain_length -= 3;

        if (r1 < mpm_length) { /* This test is required for exceptional cases in which the nearest-neighbor ordering has changed within the chain while merging within that chain */
            vector<size_t> indices(mpm_number_of_indices[r0] + mpm_number_of_indices[r1]);
            //indices.resize((mpm_number_of_indices[r0] + mpm_number_of_indices[r1]));
            //indices.clear();

            int i = 0;
            for (int j = 0; j < mpm_number_of_indices[r0]; j++) {
                indices[i] = mpm[r0][j];
                i++;
            }
            for (int j = 0; j < mpm_number_of_indices[r1]; j++) {
                indices[i] = mpm[r1][j];
                i++;
            }

            FOS[FOSs_index] = indices;
            FOSs_index++;

            double_t mul0 = ((double_t) mpm_number_of_indices[r0]) / ((double_t) mpm_number_of_indices[r0] + mpm_number_of_indices[r1]);
            double_t mul1 = ((double_t) mpm_number_of_indices[r1]) / ((double_t) mpm_number_of_indices[r0] + mpm_number_of_indices[r1]);
            for (i = 0; i < mpm_length; i++) {
                if ((i != r0) && (i != r1)) {
                    S_matrix[i][r0] = mul0 * S_matrix[i][r0] + mul1 * S_matrix[i][r1];
                    S_matrix[r0][i] = S_matrix[i][r0];
                }
            }

            mpm_new = vector<vector < int >> (mpm_length - 1);
            vector<int> mpm_new_number_of_indices(mpm_length - 1);
            int mpm_new_length = mpm_length - 1;
            for (i = 0; i < mpm_new_length; i++) {
                mpm_new[i] = mpm[i];
                mpm_new_number_of_indices[i] = mpm_number_of_indices[i];
            }

            mpm_new[r0] = vector<int>(indices.begin(), indices.end());

            mpm_new_number_of_indices[r0] = mpm_number_of_indices[r0] + mpm_number_of_indices[r1];
            if (r1 < mpm_length - 1) {
                mpm_new[r1] = mpm[mpm_length - 1];
                mpm_new_number_of_indices[r1] = mpm_number_of_indices[mpm_length - 1];

                for (i = 0; i < r1; i++) {
                    S_matrix[i][r1] = S_matrix[i][mpm_length - 1];
                    S_matrix[r1][i] = S_matrix[i][r1];
                }

                for (int j = r1 + 1; j < mpm_new_length; j++) {
                    S_matrix[r1][j] = S_matrix[j][mpm_length - 1];
                    S_matrix[j][r1] = S_matrix[r1][j];
                }
            }

            for (i = 0; i < NN_chain_length; i++) {
                if (NN_chain[i] == mpm_length - 1) {
                    NN_chain[i] = r1;
                    break;
                }
            }

            mpm = mpm_new;
            mpm_number_of_indices = mpm_new_number_of_indices;
            mpm_length = mpm_new_length;

            if (mpm_length == 1)
                done = 1;
        }
    }

    return FOS;
}

map<size_t, size_t> GOMEAFOS::node_to_depth_map = map<size_t, size_t>();
map<size_t, vector < size_t>> GOMEAFOS::depth_to_nodes_map = map<size_t, vector<size_t>>();

map<vector<size_t>, size_t> GOMEAFOS::ImprovementsByF = map<vector<size_t>, size_t>();

std::vector<std::vector<size_t> > GOMEAFOS::GenerateAllPossibleCombinationsFOS(size_t number_of_nodes) {

    assert(number_of_nodes == 15);

    GOMEAFOS::depth_to_nodes_map[0] = {0};
    GOMEAFOS::depth_to_nodes_map[1] = {1, 8};
    GOMEAFOS::depth_to_nodes_map[2] = {2, 5, 9, 12};
    GOMEAFOS::depth_to_nodes_map[3] = {3, 4, 6, 7, 10, 11, 13, 14};


    vector<vector < size_t>> FOS;

    vector<size_t> values(number_of_nodes);
    std::iota(values.begin(), values.end(), 0);

    for (size_t k = 1; k <= number_of_nodes; k++) {

        vector<size_t> combinations;
        combinations.reserve(k);

        GenerateAllCombinationsRecursive(0, k, values, combinations, FOS);
    }

    FOS.pop_back();



    for (auto it = depth_to_nodes_map.begin(); it != depth_to_nodes_map.end(); it++) {
        size_t d = it->first;
        for (size_t n : it->second) {
            node_to_depth_map[n] = d;
        }
    }

    /* map FOS to depths */
    set<vector < size_t>> DFOS;

    for (vector<size_t> & F : FOS) {
        vector<size_t> depths;
        depths.reserve(F.size());
        for (size_t n : F) {
            depths.push_back(node_to_depth_map[n]);
        }

        if (depths.size() > number_of_nodes / 2)
            continue;

        sort(depths.begin(), depths.end());

        DFOS.insert(depths);
    }

    set<vector < size_t>> SFOS;

    for (vector<size_t> DF : DFOS) {

        vector<size_t> nodes;
        nodes.reserve(DF.size());
        for (size_t d : DF) {
            //cout << d << " ";
            vector<size_t> candidates = depth_to_nodes_map[d];
            size_t chosen;
            do {
                chosen = candidates[ randu() * candidates.size() ];
            } while (find(nodes.begin(), nodes.end(), chosen) != nodes.end());
            nodes.push_back(chosen);
        }
        //cout << endl;
        sort(nodes.begin(), nodes.end());
        SFOS.insert(nodes);
    }

    FOS.clear();
    FOS = vector<vector < size_t >> (SFOS.begin(), SFOS.end());

    /*cout << endl;
    
    for (size_t i = 0; i < FOS.size(); i++) {
        for (size_t j = 0; j < FOS[i].size(); j++)
            cout << FOS[i][j] << " ";
        cout << endl;
    }
    cout << FOS.size() << endl;*/

    return FOS;
}

void GOMEAFOS::GenerateAllCombinationsRecursive(size_t offset, size_t k, std::vector<size_t> & values, std::vector<size_t> & combination, std::vector<vector<size_t>> &combinations) {
    if (k == 0) {
        combinations.push_back(combination);
        return;
    }

    for (size_t i = offset; i <= values.size() - k; i++) {
        combination.push_back(values[i]);
        GenerateAllCombinationsRecursive(i + 1, k - 1, values, combination, combinations);
        combination.pop_back();
    }

}

int GOMEAFOS::DetermineNearestNeighbour(int index, vector<vector<double_t>> &S_matrix, vector<int> & mpm_number_of_indices, int mpm_length) {
    int i, result;

    result = 0;
    if (result == index)
        result++;
    for (i = 1; i < mpm_length; i++) {
        if (((S_matrix[index][i] > S_matrix[index][result]) || ((S_matrix[index][i] == S_matrix[index][result]) && (mpm_number_of_indices[i] < mpm_number_of_indices[result]))) && (i != index))
            result = i;
    }

    return ( result);
}

std::vector<std::vector<size_t> > GOMEAFOS::GenerateLinkageTreeFOS(size_t number_of_nodes, const std::vector<Node*>& population, arma::mat * MI_distribution_adjustments, bool fos_no_root_swap) {

    vector<vector < size_t>> FOS;

    size_t pop_size = population.size();

    // phase 1: estimate similarity matrix
    vector<vector < double_t >> mi_matrix(number_of_nodes, vector<double_t>(number_of_nodes, 0.0));

    uword encode_number = 0;
    unordered_map<string, uword> values_to_int_map;
    values_to_int_map.reserve(1000000);

    // maximum number of constants to consider, the subsequent ones will be binned
    size_t max_constants = 100;
    unordered_set<double_t> constants;
    vector<double_t> constants_v;

    constants.reserve(max_constants);
    constants_v.reserve(max_constants);

    // pre-process the population: make a symbolic representation of the population nodes 
    vector<vector < uword >> pop_nodes(pop_size);

    for (size_t i = 0; i < pop_size; i++) {

        vector<uword> nodes;
        vector<Node *> nnodes = population[i]->GetSubtreeNodes(false);

        for (size_t j = 0; j < number_of_nodes; j++) {

            Node * n = nnodes[j];
            string v = n->GetValue();

            // use constant binning
            if (n->op->type == OperatorType::opTermConstant) {

                if (constants.size() < max_constants) {
                    constants.insert(((OpRegrConstant*) n->op)->GetConstant());

                    if (constants.size() >= max_constants && constants_v.empty()) {
                        constants_v.insert(constants_v.begin(), constants.begin(), constants.end());
                        sort(constants_v.begin(), constants_v.end());
                    }

                } else {
                    // find nearest constant, set it to that value
                    const double_t q = ((OpRegrConstant*) n->op)->GetConstant();
                    double_t least_dist = constants_v[0];
                    size_t least_idx = 0;
                    for (size_t const_idx = 1; const_idx < constants_v.size(); const_idx++) {
                        double_t dist = abs(q - constants_v[const_idx]);
                        if (dist < least_dist) {
                            least_dist = dist;
                            least_idx = const_idx;
                        } else {
                            // make use of sortedness of the vector
                            break;
                        }
                    }
                    v = constants_v[least_idx];
                }
            }

            auto it = values_to_int_map.find(v);
            if (it == values_to_int_map.end()) {
                values_to_int_map[v] = encode_number;

                nodes.push_back(encode_number);

                encode_number++;
            } else {
                nodes.push_back(it->second);
            }
        }

        pop_nodes[i] = nodes;
    }

    // build frequency table for symbol pairs
    mat frequencies(values_to_int_map.size(), values_to_int_map.size(), fill::zeros);
    uword val_i, val_j;

    // measure frequencies of pairs & compute joint entropy
    for (uword i = 0; i < number_of_nodes; i++) {
        for (uword j = i + 1; j < number_of_nodes; j++) {
            for (uword p = 0; p < pop_size; p++) {
                val_i = pop_nodes[p][i];
                val_j = pop_nodes[p][j];

                frequencies.at(val_i, val_j) += 1.0;
            }

            double_t freq;
            for (uword k = 0; k < encode_number; k++) {
                for (uword l = 0; l < encode_number; l++) {
                    freq = frequencies(k, l);
                    if (freq > 0.0) {
                        freq = freq / pop_size;
                        mi_matrix[i][j] += -freq * log(freq);
                        frequencies.at(k, l) = 0.0; // reset the freq;
                    }
                }
            }
            mi_matrix[j][i] = mi_matrix[i][j];
        }

        for (uword p = 0; p < pop_size; p++) {
            val_i = pop_nodes[p][i];
            frequencies.at(val_i, val_i) += 1.0;
        }

        double_t freq;
        for (uword k = 0; k < encode_number; k++) {
            for (uword l = 0; l < encode_number; l++) {
                freq = frequencies(k, l);
                if (freq > 0) {
                    freq = freq / pop_size;
                    mi_matrix[i][i] += -freq * log(freq);
                    frequencies.at(k, l) = 0.0; // reset the freq;
                }
            }
        }
    }

    // transform entropy into mutual information
    // apply correction if the pointer MI_distribution_adjustments is not null
    if (!MI_distribution_adjustments) {

        for (size_t i = 0; i < number_of_nodes; i++) {
            for (size_t j = i + 1; j < number_of_nodes; j++) {
                mi_matrix[i][j] = mi_matrix[i][i] + mi_matrix[j][j] - mi_matrix[i][j];
                mi_matrix[j][i] = mi_matrix[i][j];
            }
        }

    } else {

        if (MI_distribution_adjustments->n_elem == 0) {
            delete MI_distribution_adjustments->memptr();
            MI_distribution_adjustments->set_size(number_of_nodes, number_of_nodes);
            for (size_t i = 0; i < number_of_nodes; i++) {
                (*MI_distribution_adjustments)(i, i) = 1.0 / mi_matrix[i][i];
                for (size_t j = i + 1; j < number_of_nodes; j++) {
                    (*MI_distribution_adjustments)(i, j) = 2.0 / mi_matrix[i][j];
                }
            }
        }

        for (size_t i = 0; i < number_of_nodes; i++) {
            mi_matrix[i][i] = mi_matrix[i][i] * (*MI_distribution_adjustments)(i, i);
            for (size_t j = i + 1; j < number_of_nodes; j++) {
                mi_matrix[i][j] = mi_matrix[i][j] * (*MI_distribution_adjustments)(i, j);
            }
        }


        for (size_t i = 0; i < number_of_nodes; i++) {
            for (size_t j = i + 1; j < number_of_nodes; j++) {
                mi_matrix[i][j] = mi_matrix[i][i] + mi_matrix[j][j] - mi_matrix[i][j];
                mi_matrix[j][i] = mi_matrix[i][j];
            }
        }
    }

    // assemble the Linkage Tree with UPGMA
    FOS = BuildLinkageTreeFromSimilarityMatrix(number_of_nodes, mi_matrix);

    FOS.pop_back(); // remove the root of the Linkage Tree (otherwise entire solution can be swapped during GOM)

    if (fos_no_root_swap) {
        for (size_t i = 0; i < FOS.size(); i++) {
            vector<size_t> * set = &FOS[i];
            auto it = find(set->begin(), set->end(), 0);
            if (it != set->end()) {
                set->erase(it);
            }
        }
        // remove empty sets and duplicates
        set<vector < size_t>> fos_set;
        for (vector<size_t> set : FOS) {
            if (!set.empty())
                fos_set.insert(set);
        }
        FOS = vector<vector < size_t >> (fos_set.begin(), fos_set.end());
    }

    bool no_univar = false;
    if (no_univar) {
        set<vector < size_t>> fos_set;
        for (vector<size_t> set : FOS) {
            if (set.size() > 1)
                fos_set.insert(set);
        }
        FOS = vector<vector < size_t >> (fos_set.begin(), fos_set.end());
    }

    /*cout << " >>> generated FOS:" << endl;
    for (vector<size_t> & F : FOS) {
        if (F.size() > 1) {
            cout << "     ";
            for (size_t idx : F) {
                cout << idx << " ";
            }
            cout << endl;
        }
    }*/

    return FOS;
}

std::vector<std::vector<size_t> > GOMEAFOS::GenerateUnivariateFOS(size_t number_of_nodes, bool fos_no_root_swap) {
    vector<vector < size_t>> FOS;
    for (size_t i = 0; i < number_of_nodes; i++) {
        vector<size_t> subset = {i};
        FOS.push_back(subset);
    }

    if (fos_no_root_swap)
        FOS.erase(FOS.begin());

    return FOS;
}

std::vector<std::vector<size_t> > GOMEAFOS::GenerateRandomTreeFOS(size_t number_of_nodes, bool fos_no_root_swap) {

    // build random hierarchical FOS
    vector<vector < size_t>> FOS;
    for (size_t i = 0; i < number_of_nodes; i++) {
        vector<size_t> subset = {i};
        FOS.push_back(subset);
    }

    vector<vector < size_t >> fos_clone(FOS);
    while (fos_clone.size() > 2) {
        // take two random sets
        size_t first_set_index = randu() * fos_clone.size();
        size_t second_set_index;
        do {
            second_set_index = randu() * fos_clone.size();
        } while (second_set_index == first_set_index);

        if (first_set_index > second_set_index) {
            size_t temp = first_set_index;
            first_set_index = second_set_index;
            second_set_index = temp;
        }

        vector<size_t> merged_set;
        merged_set.reserve(fos_clone[first_set_index].size() + fos_clone[second_set_index].size());
        for (size_t v : fos_clone[first_set_index]) {
            merged_set.push_back(v);
        }
        for (size_t v : fos_clone[second_set_index]) {
            merged_set.push_back(v);
        }

        fos_clone.erase(fos_clone.begin() + first_set_index);
        fos_clone.erase(fos_clone.begin() + second_set_index - 1);

        fos_clone.push_back(merged_set);
        FOS.push_back(merged_set);
    }

    if (fos_no_root_swap) {
        for (size_t i = 0; i < FOS.size(); i++) {
            vector<size_t> * set = &FOS[i];
            auto it = find(set->begin(), set->end(), 0);
            if (it != set->end()) {
                set->erase(it);
            }
        }
        // remove empty sets and duplicates
        set<vector < size_t>> fos_set;
        for (vector<size_t> set : FOS) {
            if (!set.empty())
                fos_set.insert(set);
        }
        FOS = vector<vector < size_t >> (fos_set.begin(), fos_set.end());
    }

    return FOS;
}

std::vector<std::vector<size_t> > GOMEAFOS::GenerateConnectedNodesList(size_t max_arity, size_t max_height) {

    std::vector<std::vector < size_t>> cnl;
    size_t idx = 0;
    FillAllSubtreesIndices(idx, NULL, max_height, max_arity, cnl);

    return cnl;

}

void GOMEAFOS::FillAllSubtreesIndices(size_t & curr_index, std::vector<size_t> * parent_subtree, size_t curr_depth, size_t max_arity, std::vector<std::vector < size_t>> &cnl) {
    vector<size_t> my_subtree;
    my_subtree.push_back(curr_index);
    curr_index++;
    if (curr_depth > 0) {
        for (size_t i = 0; i < max_arity; i++)
            FillAllSubtreesIndices(curr_index, &my_subtree, curr_depth - 1, max_arity, cnl);
    }

    if (parent_subtree != NULL)
        parent_subtree->insert(parent_subtree->end(), my_subtree.begin(), my_subtree.end());


    cnl.push_back(my_subtree);
}
