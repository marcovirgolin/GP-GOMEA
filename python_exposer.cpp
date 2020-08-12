#include <cstdlib>

#include <iostream>
#include <sstream>
#include <armadillo>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/mem_fn.hpp>
#include <boost/shared_ptr.hpp>

#include "Evolution/EvolutionState.h"
#include "Evolution/EvolutionRun.h"
#include "RunHandling/IMSHandler.h"
#include "Genotype/Node.h"
#include "Utils/Utils.h"

namespace py = boost::python;
namespace np = boost::python::numpy;

struct GPGOMEA {
public:

    GPGOMEA(std::string hyperparams_string) {

        Py_Initialize();
        np::initialize();

        this->hyperparams_string = hyperparams_string;

        bool silent = false;
        silent_output_stream = new std::stringstream();

        std::vector<std::string> opts = Utils::SplitStringByChar(hyperparams_string, ' ');
        std::vector<std::string> argv_v = {"dummy"};
        argv_v.reserve(100);
        for (size_t i = 0; i < opts.size(); i++) {
            if (opts[i].compare("--silent") == 0) {
                silent = true;
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

        // setting options
        st = new EvolutionState();
        st->config->running_from_python = true;
        st->SetOptions(argc, argv);
    }

    ~GPGOMEA() {
    };

    Node * solution = NULL;
    EvolutionState * st = NULL;
    IMSHandler * imsh = NULL;
    std::string hyperparams_string = "";
    std::streambuf * default_output_stream = std::cout.rdbuf();
    std::stringstream * silent_output_stream = NULL;

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
        st->fitness->SetFitnessCases(TR, FitnessCasesType::FitnessCasesTRAIN);
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

        // delete run handler
        delete imsh;
        imsh = NULL;
    };

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
    };
    
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
        
    };

    size_t get_n_nodes(){
	return solution->GetSubtreeNodes(true).size();
    }

    size_t get_evaluations(){
        return evaluations;
    }

    /*void destroy() {
        reset();
        delete this;
    };*/

private:


};

/*boost::shared_ptr<GPGOMEA> create_GPGOMEA(std::string hyperparams_string) {
    return boost::shared_ptr<GPGOMEA>(
            new GPGOMEA(hyperparams_string),
            boost::mem_fn(&GPGOMEA::destroy));
}*/

struct GPGOMEA_pickle_suite : boost::python::pickle_suite {
    static
    boost::python::tuple
    getinitargs(GPGOMEA const& g) {
        return boost::python::make_tuple(g.hyperparams_string);
    }
};

BOOST_PYTHON_MODULE(gpgomea) {
    //py::class_<GPGOMEA, std::auto_ptr<GPGOMEA>, boost::noncopyable >
    //        ("GPGOMEA", py::no_init) //("GPGOMEA", py::init<std::string>())
    //.def("__init__", py::make_constructor(&create_GPGOMEA))

    py::class_<GPGOMEA, std::auto_ptr<GPGOMEA> >("GPGOMEA", py::init<std::string>())
            .def("run", &GPGOMEA::run)
            .def("predict", &GPGOMEA::predict)
            .def("score", &GPGOMEA::score)
            .def("get_model", &GPGOMEA::get_model)
            .def("get_evaluations", &GPGOMEA::get_evaluations)
            .def("get_n_nodes", &GPGOMEA::get_n_nodes)
            .def_pickle(GPGOMEA_pickle_suite())
            ; 
}
