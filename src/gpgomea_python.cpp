#include <cstdlib>

#include <iostream>
#include <sstream>
#include <armadillo>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/mem_fn.hpp>
#include <boost/shared_ptr.hpp>

#include "GPGOMEA/Evolution/EvolutionState.h"
#include "GPGOMEA/Evolution/EvolutionRun.h"
#include "GPGOMEA/RunHandling/IMSHandler.h"
#include "GPGOMEA/Genotype/Node.h"
#include "GPGOMEA/Utils/Utils.h"

namespace py = boost::python;
namespace np = boost::python::numpy;

struct pyGPGOMEA {
public:

    pyGPGOMEA(std::string hyperparams_string) {

        Py_Initialize();
        np::initialize();

        this->hyperparams_string = hyperparams_string;

        bool silent = false;
        silent_output_stream = new std::stringstream();
        output_file_stream = new std::ofstream();
        bool log_to_file = false;
        std::string logfile;

        std::vector<std::string> opts = Utils::SplitStringByChar(hyperparams_string, ' ');
        std::vector<std::string> argv_v = {"dummy"};
        argv_v.reserve(100);
        for (size_t i = 0; i < opts.size(); i++) {
            if (opts[i].compare("--silent") == 0) {
                silent = true;
                continue;
            }
            if (opts[i].compare("--logtofile") == 0) {
                log_to_file = true;
                logfile = opts[++i]; // get the value as next element, while also increase i as to skip the value next iteration
                continue;
            }
            argv_v.push_back(opts[i]);
        }
        size_t argc = argv_v.size();
        char * argv[argc];
        for (size_t i = 0; i < argc; i++)
            argv[i] = (char*) argv_v[i].c_str();

        if (silent) {
            std::cout.rdbuf(silent_output_stream->rdbuf());
        } else {
            std::cout.rdbuf(default_output_stream);
        }
        if (log_to_file) {
            output_file_stream = new std::ofstream(logfile.c_str());
            std::cout.rdbuf(output_file_stream->rdbuf());
        }

        // setting options
        st = new EvolutionState();
        st->config->running_from_python = true;
        st->SetOptions(argc, argv);
    }

    ~pyGPGOMEA() {
    };

    Node * solution = NULL;
    EvolutionState * st = NULL;
    IMSHandler * imsh = NULL;
    std::string hyperparams_string = "";
    std::streambuf * default_output_stream = std::cout.rdbuf();
    std::ofstream * output_file_stream = NULL;
    std::stringstream * silent_output_stream = NULL;
    std::vector<Node*> final_population;

    size_t evaluations = 0;

    void reset() {
        if (solution)
            solution->ClearSubtree();
        if (imsh)
            delete imsh;
        if (st)
            delete st;
        if (silent_output_stream)
            delete silent_output_stream;
        if (output_file_stream)
            delete output_file_stream;
        for (Node * n : final_population)
            n->ClearSubtree();
        solution = NULL;
        imsh = NULL;
        st = NULL;
        silent_output_stream = NULL;
        std::cout.rdbuf(default_output_stream);
        evaluations = 0;
    }

    void run(np::ndarray npX, np::ndarray npY) {

        arma::mat X = Utils::ConvertNumpyToArma(npX);
        arma::mat Y = Utils::ConvertNumpyToArma(npY);

        arma::mat TR = arma::join_horiz(X, Y);
        // setting validation set if validation option is on
        if (st->config->validation_perc > 0) {
            arma::mat V;
            size_t nrows_validation = TR.n_rows * st->config->validation_perc;
            while (V.n_rows < nrows_validation) {
                size_t which_row = arma::randu() * TR.n_rows;
                arma::mat x = TR.row(which_row);
                V = join_vert(V, x);
                TR.shed_row(which_row);
            }
            st->fitness->SetFitnessCases(V, FitnessCasesType::FitnessCasesVALIDATION);
        }
        st->fitness->SetFitnessCases(TR, FitnessCasesType::FitnessCasesTRAIN);

        // A test set is not used when running from Python because .fit only takes the training set
        st->fitness->SetFitnessCases(TR, FitnessCasesType::FitnessCasesTEST);

        // ADD VARIABLE TERMINALS
        for (size_t i = 0; i < TR.n_cols - 1; i++)
            st->config->terminals.push_back(new OpVariable(i));

        // ADD SR ERC
        if (st->config->use_ERC) {
            double_t biggest_val = arma::max(arma::max(arma::abs(st->fitness->TrainX)));
            double_t min_erc = -5 * biggest_val;
            double_t max_erc = 5 * biggest_val;
            st->config->terminals.push_back(new OpRegrConstant(min_erc, max_erc));
        }

        // generate run handler
        imsh = new IMSHandler(st);

        // reset state that may impact the run
        st->fitness->evaluations = 0;

        // run
        imsh->Start();

        // save anything that is interesting in the state of the object
        solution = imsh->GetFinalElitist()->CloneSubtree();
        evaluations = st->fitness->evaluations;
        final_population = imsh->GetAllActivePopulations(true);

        // delete run handler
        delete imsh;
        imsh = NULL;
    }

    np::ndarray predict(np::ndarray npX) {
        if (!solution) {
            PyErr_SetString(PyExc_TypeError, "predict error: called before fitting");
            py::throw_error_already_set();
        }

        arma::mat X = Utils::ConvertNumpyToArma(npX);

        arma::vec out = solution->GetOutput(X, false);
        std::pair<double_t, double_t> ab = std::make_pair(0.0, 1.0);
        if (st->config->linear_scaling) {
            arma::vec trainout = solution->GetOutput(st->fitness->TrainX, st->config->caching);
            ab = Utils::ComputeLinearScalingTerms(trainout, st->fitness->TrainY, &st->fitness->trainY_mean, &st->fitness->var_comp_trainY);
        }
        out = ab.first + out * ab.second;
        np::ndarray res = Utils::ToNumpyArray(out);

        return res;
    }

    double_t score(np::ndarray npX, np::ndarray npY) {
        if (!solution) {
            PyErr_SetString(PyExc_TypeError, "score error: called before fitting");
            py::throw_error_already_set();
        }

        arma::mat X = Utils::ConvertNumpyToArma(npX);
        arma::mat Y = Utils::ConvertNumpyToArma(npY);

        arma::mat TE = arma::join_horiz(X, Y);
        st->fitness->SetFitnessCases(TE, FitnessCasesType::FitnessCasesTEST);

        double_t neg_err = -1.0 * st->fitness->GetTestFit(solution);
        return neg_err;
    }

    std::string get_model() {
        if (!solution) {
            PyErr_SetString(PyExc_TypeError, "get_model error: called before fitting");
            py::throw_error_already_set();
        }

        std::string human_expression = solution->GetSubtreeHumanExpression();
        if (st->config->linear_scaling) {
            arma::vec trainout = solution->GetOutput(st->fitness->TrainX, st->config->caching);
            auto ab = Utils::ComputeLinearScalingTerms(trainout, st->fitness->TrainY, &st->fitness->trainY_mean, &st->fitness->var_comp_trainY);
            human_expression = std::to_string(ab.first) + "+" + std::to_string(ab.second) + "*(" + human_expression + ")";
        }

        return human_expression;
    }

    size_t get_n_nodes() {
	    return solution->GetSubtreeNodes(true).size();
    }

    size_t get_evaluations() {
        return evaluations;
    }

    boost::python::list get_final_population(np::ndarray npX) {

        if (!solution || final_population.empty()) {
            PyErr_SetString(PyExc_TypeError, "get_final_population error: called before fitting");
            py::throw_error_already_set();
        }

        boost::python::list return_population;

        for(Node * n : final_population) {
            // human_expression
            std::pair<double_t, double_t> ab = std::make_pair(0.0, 1.0);
            std::string prefix_expression = n->GetSubtreeExpression();
            std::string human_expression = n->GetSubtreeHumanExpression();
            if (st->config->linear_scaling) {
                arma::vec trainout = n->GetOutput(st->fitness->TrainX, st->config->caching);
                ab = Utils::ComputeLinearScalingTerms(trainout, st->fitness->TrainY, &st->fitness->trainY_mean, &st->fitness->var_comp_trainY);
                prefix_expression = "+" + std::to_string(ab.first) + "*" + std::to_string(ab.second) + prefix_expression;
                human_expression = std::to_string(ab.first) + "+" + std::to_string(ab.second) + "*(" + human_expression + ")";
            }

            // test set prediction
            arma::mat X = Utils::ConvertNumpyToArma(npX);
            arma::vec out = n->GetOutput(X, false);
            out = ab.first + out * ab.second;
            np::ndarray prediction = Utils::ToNumpyArray(out);

            // 2nd objective
            boost::python::tuple tuple;
            if (n->cached_objectives.n_elem > 1){
                double_t second_obj = n->cached_objectives[1];
                tuple = boost::python::make_tuple(prediction, n->cached_fitness, second_obj, human_expression, prefix_expression);
            }
            else {
                tuple = boost::python::make_tuple(prediction, n->cached_fitness, human_expression, prefix_expression);
            }   

            return_population.append(tuple);
        }

        return return_population;
    };

private:

};

struct GPGOMEA_pickle_suite : boost::python::pickle_suite {
    static boost::python::tuple getinitargs(pyGPGOMEA const& g) {
        return boost::python::make_tuple(g.hyperparams_string);
    }
};

BOOST_PYTHON_MODULE(gpgomea) {
    
    py::class_<pyGPGOMEA, std::auto_ptr<pyGPGOMEA> >("GPGOMEA", py::init<std::string>())
            .def("run", &pyGPGOMEA::run)
            .def("predict", &pyGPGOMEA::predict)
            .def("score", &pyGPGOMEA::score)
            .def("get_model", &pyGPGOMEA::get_model)
            .def("get_evaluations", &pyGPGOMEA::get_evaluations)
            .def("get_n_nodes", &pyGPGOMEA::get_n_nodes)
            .def("get_final_population", &pyGPGOMEA::get_final_population)
            .def_pickle(GPGOMEA_pickle_suite())
            ;
}
