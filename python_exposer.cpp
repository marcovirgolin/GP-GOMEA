#include <cstdlib>

#include <iostream>
#include <sstream>
#include <armadillo>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>


#include "Evolution/EvolutionState.h"
#include "Evolution/EvolutionRun.h"
#include "RunHandling/IMSHandler.h"
#include "Genotype/Node.h"

namespace py = boost::python;
namespace np = boost::python::numpy;

struct GPGOMEA {

    GPGOMEA() {
        Py_Initialize();
        np::initialize();
    }

    Node * solution = NULL;
    EvolutionState * st = NULL;
    IMSHandler * imsh = NULL;

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
        solution->ClearSubtree();
        delete imsh;
        delete st;
        solution = NULL;
        imsh = NULL;
        st = NULL;
    }

    void run(std::string options, bool verbose = true) {

        if (solution) {
            // reset
            reset();
        }

        std::streambuf * old = std::cout.rdbuf();
        std::stringstream * ss = NULL;
        if (!verbose) {
            ss = new std::stringstream();
            std::cout.rdbuf(ss->rdbuf());
        }

        std::vector<std::string> opts = Utils::SplitStringByChar(options, ' ');
        std::vector<std::string> argv_v = {"dummy"};
        argv_v.reserve(100);
        for (size_t i = 0; i < opts.size(); i++) {
            argv_v.push_back(opts[i]);
        }
        size_t argc = argv_v.size();
        char * argv[argc];
        for (size_t i = 0; i < argc; i++)
            argv[i] = (char*) argv_v[i].c_str();

        // setting options
        st = EvolutionState::GetInstance();
        st->SetOptions(argc, argv);

        // running
        imsh = new IMSHandler(st);
        imsh->Start();

        solution = imsh->GetFinalElitist()->CloneSubtree();

        std::cout << std::endl;
        
        if (ss) {
            delete ss;
            ss = NULL;
            std::cout.rdbuf(old);
        }
    };

    np::ndarray predict(np::ndarray npX) {

        if (!solution) {
            std::cout << "predict error: calling before having run" << std::endl;
            py::throw_error_already_set();
        }

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

};

BOOST_PYTHON_MODULE(gpgomea) {
    py::class_<GPGOMEA>("GPGOMEA")
            .def("run", &GPGOMEA::run)
            .def("predict", &GPGOMEA::predict)
            ;
}
