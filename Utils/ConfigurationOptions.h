/* 
 * File:   ConfigurationOptions.h
 * Author: virgolin
 *
 * Created on June 28, 2018, 2:14 PM
 */

#ifndef CONFIGURATIONOPTIONS_H
#define CONFIGURATIONOPTIONS_H

#include "../Operators/OpVariable.h"
#include "../GOMEA/GOMEAFOS.h"
#include "../Variation/TreeInitializer.h"
#include "../Semantics/SemanticLibrary.h"

// SYMBOLIC REGRESSION
#include "../Fitness/SymbolicRegressionFitness.h"
#include "../Operators/Regression/OpPlus.h"
#include "../Operators/Regression/OpMinus.h"
#include "../Operators/Regression/OpTimes.h"
#include "../Operators/Regression/OpAnalyticQuotient.h"
#include "../Operators/Regression/OpAnalyticQuotient_01.h"
#include "../Operators/Regression/OpExp.h"
#include "../Operators/Regression/OpLog.h"
#include "../Operators/Regression/OpAnalyticLog_01.h"
#include "../Operators/Regression/OpSin.h"
#include "../Operators/Regression/OpCos.h"
#include "../Operators/Regression/OpSquare.h"
#include "../Operators/Regression/OpSquareRoot.h"
#include "../Operators/Regression/OpRegrConstant.h"

// BOOLEAN
#include "../Operators/Boolean/OpAnd.h"
#include "../Operators/Boolean/OpNand.h"
#include "../Operators/Boolean/OpOr.h"
#include "../Operators/Boolean/OpNor.h"
#include "../Operators/Boolean/OpXor.h"
#include "../Operators/Boolean/OpNot.h"



class ConfigurationOptions {
public:

    // Add here a new operator! (of course, include the .h file above)
    std::vector<Operator *> all_operators = {new OpPlus(), new OpMinus(), new OpTimes(), new OpAnalyticQuotient(),
        new OpAnalyticQuotient01(), new OpAnalyticLog01(),
        new OpExp(), new OpLog(), new OpSin(), new OpCos(), new OpSquare(), new OpSquareRoot(),
        new OpAnd(), new OpOr(), new OpNand(), new OpNor(), new OpNot(), new OpXor()
    };

    ~ConfigurationOptions() {
        // TODO: for some reason, clearing up this memory makes python crash on subsequent instantiations
        /*for (Operator * op : all_operators) {
            delete op;
        }*/
    }

    /* Meta-Options */
    size_t rng_seed;
    size_t threads = 1;
    bool caching = false;

    /* Evolution Budget */
    int max_generations = 100;
    int max_evaluations = -1;
    int max_time = -1;

    /* Evolution Base Parameters */
    size_t population_size = 500;
    int syntactic_uniqueness_tries = -1;
    int semantic_uniqueness_tries = -1;
    std::vector<Operator *> functions;
    std::vector<Operator *> terminals;
    bool use_ERC = false;
    bool use_IMS = false;
    size_t num_sugen_IMS = 10;
    size_t early_stopping_IMS = 1;

    /* Init and Variation */
    size_t initial_maximum_tree_height = 6;
    int maximum_tree_height = 17;
    int maximum_solution_size = -1;
    TreeInitType tree_init_type = TreeInitType::TreeInitRHH;

    size_t elitism = 1;
    double_t subtree_crossover_proportion = 0.9;
    double_t subtree_mutation_proportion = 0.1;
    double_t reproduction_proportion = 0.0;

    double_t rdo_proportion = 0.0;
    double_t agx_proportion = 0.0;
    SemanticLibraryType semback_library_type = SemanticLibraryType::SemLibRandomDynamic;
    size_t semback_library_max_size = 500;
    size_t semback_library_max_height = 4;
    bool semback_normalized = false;
    bool semback_linear_lib_parse = false;

    bool semantic_variation = false;
    bool uniform_depth_variation = false;
    bool offspring_selection = false;
    // semback lib type

    // Selection
    size_t tournament_selection_size = 7;

    // GOMEA
    bool gomea = false;
    FOSType fos_type = FOSType::FOSLinkageTree;
    bool gomfos_noroot = false;
    double_t gomea_replace_worst = 0.0;

    // MultiObjective
    bool multi_objective = false;
    bool mo_size = false;

    // Extra
    bool linear_scaling = false;

    bool running_from_python = false;

};

#endif /* CONFIGURATIONOPTIONS_H */

