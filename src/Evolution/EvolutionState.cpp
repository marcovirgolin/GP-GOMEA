#include "GPGOMEA/Evolution/EvolutionState.h"

using namespace std;
using namespace arma;
namespace po = boost::program_options;

void EvolutionState::ReadAndSetDataSets(po::variables_map& vm) {

    string train = vm["train"].as<string>();
    string test;
    if (vm.count("test"))
        test = vm["test"].as<string>();
    else
        test = train;

    mat TR = fitness->ReadFitnessCases(train);
    mat TE = fitness->ReadFitnessCases(test);
    mat V;
    if (config->validation_perc > 0) {
        size_t nrows_validation = TR.n_rows * config->validation_perc;
        while (V.n_rows < nrows_validation) {
            size_t which_row = randu() * TR.n_rows;
            mat x = TR.row(which_row);
            V = join_vert(V, x);
            TR.shed_row(which_row);
        }
        fitness->SetFitnessCases(V, FitnessCasesType::FitnessCasesVALIDATION);
    }

    fitness->SetFitnessCases(TR, FitnessCasesType::FitnessCasesTRAIN);
    fitness->SetFitnessCases(TE, FitnessCasesType::FitnessCasesTEST);

    cout << "# train: " << train << " ( " << TR.n_rows << "x" << TR.n_cols - 1 << " )" << endl;
    if (!V.empty())
        cout << "# validation: " << config->validation_perc << " of train ( " << V.n_rows << "x" << V.n_cols - 1 << " )" << endl;
    cout << "# test: " << test << " ( " << TE.n_rows << "x" << TE.n_cols - 1 << " )" << endl;


    // ADD VARIABLE TERMINALS  
    for (size_t i = 0; i < TR.n_cols - 1; i++)
        config->terminals.push_back(new OpVariable(i));

    // ADD SR ERC
    if (config->use_ERC) {
        double_t biggest_val = arma::max(arma::max(arma::abs(fitness->TrainX)));
        double_t min_erc = -5 * biggest_val; //min(fitness->TrainY);
        double_t max_erc = 5 * biggest_val; //max(fitness->TrainY);
        config->terminals.push_back(new OpRegrConstant(min_erc, max_erc));
    }
}

Fitness * EvolutionState::FetchFitnessFunctionGivenProbName(string prob_name) {
    Fitness * fitness;
    if (prob_name.compare("symbreg") == 0) {
        cout << "symbolic regression ";

        fitness = new SymbolicRegressionFitness();

        // LINEAR SCALING
        if (vm.count("linearscaling")) {
            cout << "(linear scaling enabled)";
            config->linear_scaling = true;
            ((SymbolicRegressionFitness*) fitness)->use_linear_scaling = true;
        }
        
    } else if (prob_name.compare("pyprob") == 0) {
        cout << "custom python evaluation function ";

        assert(vm.count("pyprobdef"));
        std::vector<string> file_n_fun_name = Utils::SplitStringByChar(vm["pyprobdef"].as<string>(), '_');
        
        fitness = new PythonFitness();
        ((PythonFitness*) fitness)->SetPythonCallableFunction(file_n_fun_name[0], file_n_fun_name[1]);
        cout << " (module: " << file_n_fun_name[0] << ", function: " << file_n_fun_name[1] << ")";
     
    } else if (prob_name.compare("accuracy") == 0) {
        cout << "accuracy";

        fitness = new AccuracyFitness();

        arma::vec cw;

        if (vm.count("classweights")) {
            cout << " (class weights:";
            ((AccuracyFitness*) fitness)->use_weighting = true;
            if (vm["classweights"].as<string>().size() > 1) {
                cout << " " << vm["classweights"].as<string>();
                std::vector<string> weights = Utils::SplitStringByChar(vm["classweights"].as<string>(), '_');
                cw = arma::vec(weights.size());
                for (int wi = 0; wi < weights.size(); wi++) {
                    cw[wi] = stof(weights[wi]);
                }
            } else {
                cout << "auto";
            }
            cout << ")";
        }

        if (cw.n_elem > 1) {
            ((AccuracyFitness*) fitness)->SetCustomWeights(cw);
        }

    } else if (prob_name.compare("size") == 0) {
        cout << "solution size";
        fitness = new SolutionSizeFitness();
    } else if (prob_name.compare("phi") == 0) {
        cout << "interpretability phi";
        fitness = new InterpretabilityPHIFitness();
    }
    else {
        throw std::runtime_error("EvolutionState::FetchFitnessFunctionGivenProbName unrecognized problem type");
    }
    return fitness;
}

void EvolutionState::SetOptions(int argc, char* argv[]) {

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "show usage")
            ("file", po::value<string>(), "reads parameters from file rather than command line")
            ("seed", po::value<size_t>(), "sets the random seed (default is random)")
            ("parallel", po::value<size_t>(), "sets the number of threads for parallel computation (default is 1)")
            ("caching", "enables the caching of node outputs")
            ("generations", po::value<int>(), "sets generations limit (default 100)")
            ("evaluations", po::value<int>(), "sets evaluations limit (default disabled)")
            ("time", po::value<int>(), "sets time limit [seconds] (default disabled)")
            ("prob", po::value<string>(), "sets the problem (e.g., symbreg, multiobj)")
            ("multiobj", po::value<string>(), "sets the objectives for multiobj problem (e.g., 'symbreg_phi')")
            ("functions", po::value<string>(), "sets the functions to use")
            ("terminals", po::value<string>(), "sets the terminals to use")
            ("erc", "uses ephemeral random constants (default: enabled, sampled from -5*max(abs(features)) to 5*max(abs(features))")
            ("train", po::value<string>(), "sets the training set for symbolic regression")
            ("test", po::value<string>(), "sets the testing set for symbolic regression")
            ("validation", po::value<double_t>(), "sets the percentage of examples to take out from the training set and use for validation of best solutions in symbolic regression (default is 0)")
            ("popsize", po::value<size_t>(), "sets the size of the population (default is 500)")
            ("ims", po::value<string>(), "uses the interleaved multi-start scheme (default subgenerations number is 10, default early stopping criteria is 1; if validation is set, uses validation performance as metric)")
            ("initmaxtreeheight", po::value<size_t>(), "sets the initial maximum tree height (default is 6, minimum is 2)")
            ("inittype", po::value<string>(), "sets the initialization type (default is RHH for tree-based GP, HH for GOMEA)")
            ("syntuniqinit", po::value<int>(), "forces solutions in the initial population to be syntactically unique, with a maximum number of tries (default is disabled)")
            ("semuniqinit", po::value<int>(), "forces solutions in the initial population to be semantically unique, with a maximum number of tries (default is disabled)")
            ("maxtreeheight", po::value<int>(), "sets the maximum tree height (default is 17)")
            ("maxsize", po::value<int>(), "sets the maximum tree size, i.e., number of nodes (default is -1, disabled)")
            ("subcross", po::value<double_t>(), "sets the proportion of parents for subtree crossover (default is 0.9)")
            ("submut", po::value<double_t>(), "sets the proportion of parents for subtree mutation (default is 0.1)")
            ("reproduction", po::value<double_t>(), "sets the proportion of parents for reproduction (default is 0.0)")
            ("elitism", po::value<size_t>(), "sets the number of best solutions to keep (default is 0)")
            ("sbrdo", po::value<double_t>(), "sets the proportion of parents for RDO (default is 0.0)")
            ("sbagx", po::value<double_t>(), "sets the proportion of parents for AGX (default is 0.0)")
            ("sblibtype", po::value<string>(), "sets the type of library for Semantic Backpropagation (default is RD, max tree height 4, size 500, w/o normalization, w k-d tree)")
            ("coeffmut", po::value<string>(), "randomly mutates coefficients (constants) with the specified scheduling, either ProbMut_ES (the string 'ES' specifies that adaptive, ES-like constant optimization is used) or ProbMut_InitialMutStrength_DecayRate_NumGenWithoutImprovementBeforeDecay (default is disabled)")
            ("unifdepthvar", "picks nodes for subtree variation at uniform random depth (default is disabled)")
            ("tournament", po::value<size_t>(), "sets the size of tournament selection (default is 4)")
            ("gomea", "runs GP-GOMEA instead of tree-based GP")
            ("gomfos", po::value<string>(), "sets the FOS for Gene-pool Optimizal Mixing (default is LT: Linkage Tree)")
            ("gomfosnorootswap", "removes the root from the indices to swap in the FOS (enhances diversity, default is disabled)")
            ("gomeareplaceworst", po::value<double_t>(), "rate of worse performing population to remove and replace with new random solutions (default 0.0)")
            ("nongomcoeffmut", "applies coefficient mutation *after* GOM instead of *within* GOM (default is disabled)")
            ("batchsize", po::value<int>(), "uses batches of the specified size instead of the whole training set (default is disabled)")
            ("linearscaling", "enables linear scaling in symbolic regression (defeault is disabled)")
            ("classweights", po::value<string>(), "use class weighting for classification (default is disabled, use a single '_' to set to training set distribution, else specify manually by underscore-separated weights)")
            ("pyprobdef", po::value<string>(), "necessary to set name of module and name of function to use to evaluate individuals (separated by '_', so do not use it in their names them)");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("file")) {
        auto file = vm["file"].as<string>();
        vm.clear();
        SetOptionsFromFile(file);
        return;
    }

    if (vm.count("help")) {
        cout << desc << "\n";
        exit(0);
    }

    cout << "######################################### SETTINGS #######################################" << endl;

    // SEED
    if (vm.count("seed")) {
        config->rng_seed = vm["seed"].as<size_t>();
    } else {
        srand(time(NULL));
        config->rng_seed = rand() % 999999;
    }

    arma_rng::set_seed(config->rng_seed);
    cout << "# rng seed set to " << config->rng_seed << endl;

    // THREADS
    omp_set_dynamic(0); // Explicitly disable dynamic threads
    if (vm.count("parallel")) {
        config->threads = vm["parallel"].as<size_t>();
        cout << "# parallel on " << config->threads << " threads" << endl;
    }
    omp_set_num_threads(config->threads);


    // CACHING
    if (!vm.count("caching") && vm.count("gomfos")) { // triggers
        if (vm["gomfos"].as<string>().compare("SC") == 0 ||
                vm["gomfos"].as<string>().compare("SP") == 0 ||
                vm["gomfos"].as<string>().compare("SL") == 0)
            vm.insert(make_pair("caching", po::variable_value()));
    }
    if ((vm.count("caching"))) {
        cout << "# node output caching enabled" << endl;
        config->caching = true;
    }

    // MAX GENERATIONS
    if (vm.count("generations")) {
        config->max_generations = vm["generations"].as<int>();
        cout << "# max generations " << config->max_generations << endl;
    }

    // MAX EVALUATIONS
    if (vm.count("evaluations")) {
        config->max_evaluations = vm["evaluations"].as<int>();
        cout << "# max evaluations " << config->max_evaluations << endl;
    }

    // MAX TIME
    if (vm.count("time")) {
        config->max_time = vm["time"].as<int>();
        cout << "# max time " << config->max_time << " (s)" << endl;
    }

    // SETS FUNCTIONS AND TERMINALS
    vector<string> str_functions = Utils::SplitStringByChar(vm["functions"].as<string>(), '_');
    vector<string> str_terminals;
    if (vm.count("terminals"))
        str_terminals = Utils::SplitStringByChar(vm["terminals"].as<string>(), '_');

    for (Operator * op : config->all_operators) {
        for (string s : str_functions) {
            if (s.compare(op->name) == 0) {
                config->functions.push_back(op->Clone());
            }
        }
        for (string s : str_terminals) {
            if (s.compare(op->name) == 0) {
                config->terminals.push_back(op->Clone());
            }
        }
    }

    // Add constant terminals
    for (string s : str_terminals) {
        if (Utils::IsNumber(s)) {
            double_t constant = stof(s);
            config->terminals.push_back(new OpRegrConstant(constant));
        }
    }

    if (vm.count("erc")) {
        config->use_ERC = true;
        // is set later based on feature values
    }

    // PROBLEM
    assert(vm.count("prob"));
    string prob_name = vm["prob"].as<string>();
    if (prob_name.compare("multiobj") == 0) {
        cout << "# problem: multi-objective [";
        vector<string> sub_probs = Utils::SplitStringByChar(vm["multiobj"].as<string>(), '_');
        vector<Fitness*> sub_fitnesses; sub_fitnesses.reserve(sub_probs.size());
        for(string sub_prob : sub_probs) {
            cout << "<";
            sub_fitnesses.push_back(FetchFitnessFunctionGivenProbName(sub_prob));
            cout << ">";
        }
        cout << "]";
        this->fitness = new MOFitness(sub_fitnesses);
    } else {
        cout << "# problem: ";
        this->fitness = FetchFitnessFunctionGivenProbName(prob_name);
    }
    cout << endl;

    // Set % of examples to internally use for validation 
    // (it is done here because it can be used from Python as well)
    if (vm.count("validation")){
        config->validation_perc = vm["validation"].as<double_t>();
    }
    if (!config->running_from_python) {
        ReadAndSetDataSets(vm);
    }

    // POPSIZE 
    if (vm.count("popsize")) {
        config->population_size = vm["popsize"].as<size_t>();
    }
    cout << "# population size: " << config->population_size << endl;

    // IMS
    if (vm.count("ims")) {

        if (prob_name.compare("multiobj")==0){
            throw std::runtime_error("EvolutionState::SetOptions IMS not impletemented for multi-obj problems");
        }

        config->use_IMS = true;
        vector<string> ims_params = Utils::SplitStringByChar(vm["ims"].as<string>(), '_');
        config->num_sugen_IMS = stoi(ims_params[0]);
        config->early_stopping_IMS = stoi(ims_params[1]);

        cout << "# IMS active with ";
        cout << config->num_sugen_IMS << " number of sub-generations and " << config->early_stopping_IMS << " number of generations for early stopping" << endl;
    }

    // INITIAL MAXIMUM TREE HEIGHT
    if (vm.count("initmaxtreeheight")) {
        config->initial_maximum_tree_height = vm["initmaxtreeheight"].as<size_t>();
        if (config->initial_maximum_tree_height < 2)
            throw std::runtime_error("EvolutionState::SetOptions initial maximum height must be >= 2");
    }
    cout << "# initial maximum tree height: " << config->initial_maximum_tree_height << endl;

    // INITIALIZATION TYPE
    if (vm.count("inittype")) {
        string init_type = vm["inittype"].as<string>();
        cout << "# initialization type: ";
        if (init_type.compare("RHH") == 0) {
            config->tree_init_type = TreeInitType::TreeInitRHH;
            cout << "Ramped Half-n-Half";
        } else if (init_type.compare("HH") == 0) {
            config->tree_init_type = TreeInitType::TreeInitHH;
            cout << "Half-n-Half";
        } else if (Utils::ToLowerCase(init_type).compare("heuristic")==0) {
            config->tree_init_type = TreeInitType::TreeInitHeuristic;
            cout << "Heuristic";
        } else {
            throw std::runtime_error("EvolutionState::SetOptions unrecognized initialization type");
        }
        cout << endl;
    }

    // SEMANTIC UNIQUENESS INITIALIZATION
    if (vm.count("semuniqinit")) {
        config->semantic_uniqueness_tries = vm["semuniqinit"].as<int>();
        cout << "# semantical uniqueness initialization tries: " << config->semantic_uniqueness_tries << endl;
    }// SYNTACTICAL UNIQUENESS INITIALIZATION
    else if (vm.count("syntuniqinit")) {
        config->syntactic_uniqueness_tries = vm["syntuniqinit"].as<int>();
        cout << "# syntactical uniqueness initialization tries: " << config->syntactic_uniqueness_tries << endl;
    }

    // SEMANTIC VARIATION 
    if (vm.count("sblibtype") || (vm.count("sbrdo") && vm["sbrdo"].as<double_t>() > 0) || (vm.count("sbagx") && vm["sbagx"].as<double_t>() > 0)) // triggers
        config->semantic_variation = true;

    if (config->semantic_variation) {
        semantic_backprop = new SemanticBackpropagator();
        semantic_library = new SemanticLibrary();
    }

    // MAXIMUM TREE HEIGHT
    if (vm.count("maxsize")) {
        config->maximum_solution_size = vm["maxsize"].as<int>();
    }
    if (config->maximum_solution_size > 0)
        cout << "# maximum solution size: " << config->maximum_solution_size << endl;

    // COEFFICIENT MUTATION
    if (vm.count("coeffmut")) {
        vector<string> coeff_mut_info = Utils::SplitStringByChar(vm["coeffmut"].as<string>(), '_');
        assert(coeff_mut_info.size() == 4 || coeff_mut_info.size() == 2);
        config->coeff_mut_prob = atof(coeff_mut_info[0].c_str());
        cout << "# coefficient mutation: prob "<< config->coeff_mut_prob;
        if(Utils::IsNumber(coeff_mut_info[1])) {
            config->coeff_mut_strength = atof(coeff_mut_info[1].c_str());
            config->coeff_mut_decay = atof(coeff_mut_info[2].c_str());
            config->coeff_mut_num_gen_no_impr_decay = atoi(coeff_mut_info[3].c_str());
            cout << ", strength " << config->coeff_mut_strength;
            cout << ", decay " << config->coeff_mut_decay;
            cout << ", num. gen. w/o imprevement before decay " << config->coeff_mut_num_gen_no_impr_decay;
        } else {
            if (Utils::ToLowerCase(coeff_mut_info[1]).compare("es") != 0) {
                throw runtime_error("Unrecognized coeff. mut. option "+coeff_mut_info[1]);
            }
            config->coeff_mut_strength = -1;
            config->coeff_mut_decay = 1;
            config->coeff_mut_num_gen_no_impr_decay = -1;
            cout << ", ES-like adaptive";
        }
        cout << endl;
    }

    // BATCHING
    if (vm.count("batchsize")) {
        config->batch_size = vm["batchsize"].as<int>();
        // no elitism allowed in this case
        if (vm.count("elitism")) {
            if(vm["elitism"].as<size_t>()>0) {
                throw std::runtime_error("Elitism (>0) is not compatible with batching");
            }
        }
        // no caching allowed in this case
        if (vm.count("caching")) {
            throw std::runtime_error("Caching of outputs is not compatible with batching");
        }
        cout << "# batch size: " << config->batch_size << endl;
    }

    // GOMEA
    if (vm.count("gomea")) {
        if (prob_name.compare("multiobj")==0){
            throw std::runtime_error("EvolutionState::SetOptions Multi-objective GOMEA not impletemented");
        }
        config->gomea = true;
        cout << "# evolutionary algorithm: GOMEA";
        if (vm.count("gomfos")) {
            string fos = vm["gomfos"].as<string>();
            if (fos.compare("U") == 0) {
                cout << " (FOS: Univariate)";
                config->fos_type = FOSType::FOSUnivariate;
            } else if (fos.compare("LT") == 0) {
                cout << " (FOS: Linkage Tree)";
                config->fos_type = FOSType::FOSLinkageTree;
            } else if (fos.compare("RT") == 0) {
                cout << " (FOS: Random Tree)";
                config->fos_type = FOSType::FOSRandomTree;
            } else {
                throw std::runtime_error("EvolutionState::SetOptions unrecognized FOS type");
            }
        } else {
            cout << " (FOS: Linkage Tree)";
        }
        cout << endl;
        if (vm.count("gomfosnorootswap")) {
            config->gomfos_noroot = true;
            cout << "# GOMEA FOS no root swap" << endl;
        }

        if (vm.count("gomeareplaceworst")) {
            config->gomea_replace_worst = vm["gomeareplaceworst"].as<double_t>();
            cout << "# GOMEA replace worst " << config->gomea_replace_worst * 100 << "% of the population" << endl;
        }

        if (vm.count("nongomcoeffmut")) {
            config->nongom_coeff_mut = true;
        }

        if (!vm.count("inittype")) {
            config->tree_init_type = TreeInitType::TreeInitHH;
        }

        tree_initializer = new GOMEATreeInitializer(config->tree_init_type);
        generation_handler = new GOMEAGenerationHandler(config, tree_initializer, fitness, semantic_library);

    } else {

        tree_initializer = new TreeInitializer(config->tree_init_type);
        if (prob_name.compare("multiobj") == 0){
            generation_handler = new NSGA2GenerationHandler(config, tree_initializer, fitness, semantic_library, semantic_backprop);
            cout << "# evolutionary algorithm: tree-based NSGA-II GP " << endl;
        }
        else {
            generation_handler = new GenerationHandler(config, tree_initializer, fitness, semantic_library, semantic_backprop);
            cout << "# evolutionary algorithm: tree-based Standard GP" << endl;
        }


        // MAXIMUM TREE HEIGHT
        if (vm.count("maxtreeheight")) {
            config->maximum_tree_height = vm["maxtreeheight"].as<int>();
            if (config->maximum_tree_height < 0) {
                config->maximum_tree_height = 99999;
            }
        }
        if (config->maximum_tree_height > 0 && config->maximum_tree_height < 99999)
        cout << "# maximum tree height: " << config->maximum_tree_height << endl;



        // SUBTREE CROSSOVER
        if (vm.count("subcross")) {
            config->subtree_crossover_proportion = vm["subcross"].as<double_t>();
        }
        cout << "# subtree crossover probability: " << config->subtree_crossover_proportion << endl;

        // SUBTREE MUTATION
        if (vm.count("submut")) {
            config->subtree_mutation_proportion = vm["submut"].as<double_t>();
        }
        cout << "# subtree mutation probability: " << config->subtree_mutation_proportion << endl;

        // REPRODUCTION
        if (vm.count("reproduction")) {
            config->reproduction_proportion = vm["reproduction"].as<double_t>();
        }
        cout << "# reproduction probability: " << config->reproduction_proportion << endl;


        // SEMANTIC BACKPROPAGATION RDO
        if (vm.count("sbrdo")) {
            config->rdo_proportion = vm["sbrdo"].as<double_t>();
            cout << "# RDO probability: " << config->rdo_proportion << endl;
        }

        // SEMANTIC BACKPROPAGATION AGX
        if (vm.count("sbagx")) {
            config->agx_proportion = vm["sbagx"].as<double_t>();
            cout << "# AGX probability: " << config->agx_proportion << endl;
        }


        // SEMANTIC BACKPROPAGATION LIBRARY
        if (config->semantic_variation) {

            cout << "# Semantic backpropagation library: ";
            if (vm.count("sblibtype")) {
                vector<string> str_lib_params = Utils::SplitStringByChar(vm["sblibtype"].as<string>(), '_');
                if (Utils::ToLowerCase(str_lib_params[0]).compare("rd") == 0) {
                    config->semback_library_type = SemanticLibraryType::SemLibRandomDynamic;
                    cout << "RD, ";
                } else if (Utils::ToLowerCase(str_lib_params[0]).compare("rs") == 0) {
                    config->semback_library_type = SemanticLibraryType::SemLibRandomStatic;
                    cout << "RS, ";
                } else if (Utils::ToLowerCase(str_lib_params[0]).compare("p") == 0) {
                    config->semback_library_type = SemanticLibraryType::SemLibPopulation;
                    cout << "P, ";
                } else {
                    throw std::runtime_error("EvolutionState::SetOptions unrecognized semantic library type");
                }
                config->semback_library_max_height = stoi(str_lib_params[1]);
                config->semback_library_max_size = stoi(str_lib_params[2]);
                cout << "height=" << config->semback_library_max_height << ", size=" << config->semback_library_max_size;

                if (str_lib_params.size() > 3) {
                    for (size_t p = 3; p < str_lib_params.size(); p++) {
                        if (Utils::ToLowerCase(str_lib_params[p]).compare("l") == 0) {
                            config->semback_linear_lib_parse = true;
                            cout << ", linear library parse";
                            semantic_library->linear_parse_library = true;
                        } else if (Utils::ToLowerCase(str_lib_params[p]).compare("n") == 0) {
                            config->semback_normalized = true;
                            cout << ", normalization enabled";
                            semantic_library->normalize_outputs = true;
                        }
                    }
                }
                cout << endl;
            }

        }

        // TOURNAMENT SELECTION
        if (vm.count("tournament")) {
            config->tournament_selection_size = vm["tournament"].as<size_t>();
        }
        cout << "# tournament selection size: " << config->tournament_selection_size << endl;

    }

    // ELITISM
    if (vm.count("elitism")) {
        config->elitism = vm["elitism"].as<size_t>();
    }
    cout << "# elitism: " << config->elitism << endl;

    // UNIFORM DEPTH VARIATION
    if (vm.count("unifdepthvar")) {
        config->uniform_depth_variation = true;
        cout << "# uniform depth variation: enabled" << endl;
    }

    cout << "##########################################################################################" << endl;

}

void EvolutionState::SetOptionsFromFile(string filename) {

    string line;
    vector<string> argv_v = {"dummy"};
    argv_v.reserve(100);
    vector<string> split_line;
    ifstream file(filename);
    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.empty() || line[0] == '#')
                continue;
            if (line[0] && line.find("file") != std::string::npos) {
                continue;
            };
            line = "--" + line;
            split_line = Utils::SplitStringByChar(line, ' ');
            argv_v.insert(argv_v.end(), split_line.begin(), split_line.end());
        }
        file.close();
    } else {
        runtime_error("EvolutionState::SetOptionsFromFile unable to open file " + filename);
    }

    size_t argc = argv_v.size();
    char * argv[argc];
    for (size_t i = 0; i < argc; i++)
        argv[i] = (char*) argv_v[i].c_str();

    SetOptions(argc, argv);

}
