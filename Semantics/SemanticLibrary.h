/*
 


 */

/* 
 * File:   SemanticLibrary.h
 * Author: virgolin
 *
 * Created on July 5, 2018, 9:11 PM
 */

#ifndef SEMANTICLIBRARY_H
#define SEMANTICLIBRARY_H


#include "../Operators/Operator.h"
#include "../Fitness/Fitness.h"
#include "../Variation/TreeInitializer.h"
#include "../Utils/KDTree.h"
#include "../Genotype/Node.h"
#include "../Utils/Utils.h"
#include "../Operators/Regression/OpRegrConstant.h"

#include <vector>
#include <armadillo>
#include <map>
#include <set>

enum SemanticLibraryType {
    SemLibRandomStatic, SemLibRandomDynamic,
    SemLibPopulation
};

class SemanticLibrary {
public:

    SemanticLibrary(bool normalize_outputs = false, bool linear_parse_library = false);

    virtual ~SemanticLibrary();

    void GenerateRandomLibrary(size_t max_height, size_t max_library_size, Fitness & fitness, const std::vector<Operator*> & functions, const std::vector<Operator*> & terminals, const TreeInitializer & tree_init, bool caching);
    void GeneratePopulationLibrary(size_t max_height, size_t max_library_size, const std::vector<Node*> & population, Fitness & fitness, bool caching);

    std::pair<Node*, double_t> GetClosestSubtree(const arma::vec & x, size_t max_height, std::pair<double_t,double_t> * linear_scaling_coeffs = NULL, bool avoid_zero = false);
    std::pair<Node*, double_t> GetClosestSubtree(const std::vector< arma::vec > & x, size_t max_height, std::pair<double_t,double_t> * linear_scaling_coeffs = NULL, bool avoid_zero = false);
    std::pair<Node*, double_t> GetClosestSubtreeStochastic(const arma::vec & x, size_t max_height);
    std::pair<Node*, double_t> GetClosestSubtreeStochastic(const std::vector< arma::vec > & x, size_t max_height);

    std::map<size_t, kdt::KDTree> libraries_kdtrees;
    std::map<size_t, std::vector< std::pair<Node *, arma::vec> >> libraries_vectors;
    std::map<size_t, std::pair< Node *, arma::vec >> stored_solutions;
    std::map<Node*, std::pair<double_t, double_t>> normalization_stored_solutions_mean_std;
    std::set<size_t> already_stored_semantics;

    size_t max_tries = 100000;
    bool normalize_outputs = false;
    bool linear_parse_library = false;

private:

    bool AddToLibrary(Node * n, Fitness & fitness, bool caching);

};

#endif /* SEMANTICLIBRARY_H */

