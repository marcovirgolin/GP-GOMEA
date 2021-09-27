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

#include "GPGOMEA/Operators/Operator.h"
#include "GPGOMEA/Operators/OpVariable.h"
#include "GPGOMEA/Variation/TreeInitializer.h"
#include "GPGOMEA/Fitness/Fitness.h"
#include "GPGOMEA/Utils/Utils.h"
#include "GPGOMEA/Evolution/GenerationHandler.h"
#include "GPGOMEA/Evolution/NSGA2GenerationHandler.h"
#include "GPGOMEA/Utils/ConfigurationOptions.h"

// PROB TYPES
#include "GPGOMEA/Fitness/SymbolicRegressionFitness.h"
#include "GPGOMEA/Fitness/AccuracyFitness.h"
#include "GPGOMEA/Fitness/PythonFitness.h"
#include "GPGOMEA/Fitness/MOFitness.h"
#include "GPGOMEA/Fitness/SolutionSizeFitness.h"
#include "GPGOMEA/Fitness/InterpretabilityPHIFitness.h"

// GOMEA
#include "GPGOMEA/GOMEA/GOMEATreeInitializer.h"
#include "GPGOMEA/GOMEA/GOMEAFOS.h"
#include "GPGOMEA/GOMEA/GOMEAGenerationHandler.h"

// OTHER
#include "GPGOMEA/Semantics/SemanticLibrary.h"

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

    ~EvolutionState() {
        delete config;

        for (Node * n : population)
            n->ClearSubtree();
        delete tree_initializer;
        delete fitness;
        delete generation_handler;

        if (semantic_backprop)
            delete semantic_backprop;
        if (semantic_library)
            delete semantic_library;
    }

    void SetOptions(int argc, char* argv[]);

    Fitness * FetchFitnessFunctionGivenProbName(std::string prob_name);

    boost::program_options::variables_map vm;
    
    std::vector<Node *> population;

    // OBJECTS
    ConfigurationOptions * config;
    TreeInitializer * tree_initializer;
    Fitness * fitness;
    GenerationHandler * generation_handler;
    SemanticBackpropagator * semantic_backprop = NULL;
    SemanticLibrary * semantic_library = NULL;

    arma::wall_clock timer;

    EvolutionState() {
        config = new ConfigurationOptions();
    };

private:

    void ReadAndSetDataSets(boost::program_options::variables_map& vm);
    void SetOptionsFromFile(std::string filename);

};


#endif /* EVOLUTIONSTATE_H */

