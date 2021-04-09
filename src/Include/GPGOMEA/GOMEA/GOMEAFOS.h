/*
 


 */

/* 
 * File:   GOMEAFOS.h
 * Author: virgolin
 *
 * Created on June 28, 2018, 5:40 PM
 */

#ifndef GOMEAFOS_H
#define GOMEAFOS_H

#include "GPGOMEA/Operators/Operator.h"
#include "GPGOMEA/Operators/Regression/OpRegrConstant.h"
#include "GPGOMEA/Genotype/Node.h"
#include "GPGOMEA/Fitness/Fitness.h"
#include "GPGOMEA/Utils/Utils.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <set>
#include <armadillo>
#include <map>


enum FOSType {
    FOSLinkageTree, FOSRandomTree, FOSUnivariate
};

class GOMEAFOS {
public:

    GOMEAFOS() {
    };

    static std::vector<std::vector<size_t>> GenerateFOS(const std::vector<Node *> & population, FOSType fos_type, bool fos_no_root_swap = false, arma::mat * MI_distribution_adjustments = NULL);
    static std::vector<std::vector<size_t>> GenerateAllPossibleCombinationsFOS(size_t number_of_nodes);
    static std::vector<std::vector<size_t>> GenerateTreeTayloredFOS(const std::vector<Node *> & population);

    static std::map< std::vector<size_t>, size_t > ImprovementsByF;
    static std::map<size_t, size_t> node_to_depth_map;
    static std::map<size_t, std::vector < size_t>> depth_to_nodes_map;

private:

    static std::vector<std::vector<size_t>> GenerateLinkageTreeFOS(size_t number_of_nodes, const std::vector<Node*>& population, arma::mat * MI_distribution_adjustments, bool fos_no_root_swap = false);
    static std::vector<std::vector<size_t>> GenerateUnivariateFOS(size_t number_of_nodes, bool fos_no_root_swap);
    static std::vector<std::vector<size_t>> GenerateRandomTreeFOS(size_t number_of_nodes, bool fos_no_root_swap);
    
    static std::vector<std::vector<size_t>> BuildLinkageTreeFromSimilarityMatrix(size_t number_of_nodes, std::vector<std::vector<double_t>> &sim_matrix);
    static int DetermineNearestNeighbour(int index, std::vector<std::vector<double_t>> &S_matrix, std::vector<int> & mpm_number_of_indices, int mpm_length);

    static std::vector<std::vector<size_t> > GenerateConnectedNodesList(size_t max_arity, size_t max_height);
    static void FillAllSubtreesIndices(size_t & curr_index, std::vector<size_t> * parent_subtree, size_t curr_depth, size_t max_arity, std::vector<std::vector<size_t>> &connected_nodes_list);

    static void GenerateAllCombinationsRecursive(size_t offset, size_t k, std::vector<size_t> & values, std::vector<size_t> & combination, std::vector<std::vector<size_t>> &combinations);
};

#endif /* GOMEAFOS_H */

