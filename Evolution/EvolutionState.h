/*
 


 */

/* 
 * File:   EvolutionState.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 6:41 PM
 */

#ifndef EVOLUTIONSTATE_H
#define EVOLUTIONSTATE_H

#include "../Operators/Operator.h"
#include "../Operators/OpVariable.h"
#include "../Variation/TreeInitializer.h"
#include "../Fitness/Fitness.h"
#include "../Utils/Utils.h"
#include "../Evolution/GenerationHandler.h"
#include "../Utils/ConfigurationOptions.h"

// PROB TYPES
#include "../Fitness/SymbolicRegressionFitness.h"
#include "../Fitness/SymbolicRegressionLinearScalingFitness.h"

// GOMEA
#include "../GOMEA/GOMEATreeInitializer.h"
#include "../GOMEA/GOMEAFOS.h"
#include "../GOMEA/GOMEAGenerationHandler.h"

// OTHER
#include "../Semantics/SemanticLibrary.h"

#include <boost/program_options.hpp>
#include <armadillo>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <time.h>

class EvolutionState {
public:

    std::vector<Operator*> all_operators = {new OpPlus(), new OpMinus(), new OpTimes(), new OpAnalyticQuotient(),
        new OpAnalyticQuotient01(), new OpAnalyticLog01(),
        new OpExp(), new OpLog(), new OpSin(), new OpCos(), new OpSquare(), new OpSquareRoot()};

    static EvolutionState * GetInstance() {
        if (!instance)
            instance = new EvolutionState();
        return instance;
    }

    ~EvolutionState() {
        for (Node * n : population)
            n->ClearSubtree();
        delete tree_initializer;
        delete fitness;
        delete generation_handler;

        if (semantic_backprop)
            delete semantic_backprop;
        if (semantic_library)
            delete semantic_library;
        
        for (Operator * op : all_operators) {
            delete op;
        }
    }

    void SetOptions(int argc, char* argv[]);

    ConfigurationOptions * config = new ConfigurationOptions();

    std::vector<Node *> population;

    // OBJECTS
    TreeInitializer * tree_initializer;
    Fitness * fitness;
    GenerationHandler * generation_handler;
    SemanticBackpropagator * semantic_backprop = NULL;
    SemanticLibrary * semantic_library = NULL;

    arma::wall_clock timer;

private:

    EvolutionState() {
    };
    static EvolutionState * instance;

    void SetOptionsFromFile(std::string filename);
};


#endif /* EVOLUTIONSTATE_H */

