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

    template <class T>
    py::list toPythonList(std::vector<T> vector) {
        typename std::vector<T>::iterator iter;
        boost::python::list list;
        for (iter = vector.begin(); iter != vector.end(); ++iter) {
            list.append(*iter);
        }
        return list;
    }

    py::list toPythonList(arma::vec vector) {
        std::vector<double_t> x;
        x.reserve(vector.n_elem);
        for (size_t i = 0; i < vector.n_elem; i++)
            x.push_back(vector[i]);
        py::list result = toPythonList(x);
        return result;
    }

    template <class T>
    np::ndarray toNumpyArray(std::vector<T> vector) {
        Py_intptr_t shape[1] = {vector.size()};
        np::ndarray result = np::zeros(1, shape, np::dtype::get_builtin<double_t>());
        std::copy(vector.begin(), vector.end(), reinterpret_cast<double_t*> (result.get_data()));
        return result;
    }

    np::ndarray toNumpyArray(arma::vec vector) {
        std::vector<double_t> x;
        x.reserve(vector.n_elem);
        for (size_t i = 0; i < vector.n_elem; i++)
            x.push_back(vector[i]);
        np::ndarray result = toNumpyArray(x);
        return result;
    }

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

    arma::mat convertNumpyToArma(np::ndarray npX) {
        if (npX.get_dtype() != np::dtype::get_builtin<double_t>()) {
            PyErr_SetString(PyExc_TypeError, "predict error: wrong data type");
            py::throw_error_already_set();
        }
        if (npX.get_nd() != 2) {
            PyErr_SetString(PyExc_TypeError, "predict error: wrong number of dimensions (2)");
            py::throw_error_already_set();
        }

        Py_intptr_t const * shape = npX.get_shape();
        Py_intptr_t const * strides = npX.get_strides();
        char const * data = npX.get_data();
        arma::mat X(shape[0], shape[1]);
        for (size_t i = 0; i < shape[0]; i++) {
            for (size_t j = 0; j < shape[1]; j++) {
                X(i, j) = *reinterpret_cast<double_t const *> (data + i * strides[0] + j * strides[1]);
            }
        }
        return X;
    }

    void run(np::ndarray npX, np::ndarray npY) {

        arma::mat X = convertNumpyToArma(npX);
        arma::mat Y = convertNumpyToArma(npY);

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

        arma::mat X = convertNumpyToArma(npX);

        arma::vec out = solution->GetOutput(X, false);
        std::pair<double_t, double_t> ab = std::make_pair(0.0, 1.0);
        if (st->config->linear_scaling) {
            arma::vec trainout = solution->GetOutput(st->fitness->TrainX, st->config->caching);
            ab = Utils::ComputeLinearScalingTerms(trainout, st->fitness->TrainY, &st->fitness->trainY_mean, &st->fitness->var_comp_trainY);
        }
        out = ab.first + out * ab.second;
        np::ndarray res = toNumpyArray(out);

        return res;
    };
    
    double_t score(np::ndarray npX, np::ndarray npY) {
        
        if (!solution) {
            PyErr_SetString(PyExc_TypeError, "score error: called before fitting");
            py::throw_error_already_set();
        }
        
        arma::mat X = convertNumpyToArma(npX);
        arma::mat Y = convertNumpyToArma(npY);
        
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

        return solution->GetSubtreeHumanExpression();
    };

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
            .def_pickle(GPGOMEA_pickle_suite())
            ;
    
}
