/*
 


 */

/* 
 * File:   PythonFitness.cpp
 * Author: virgolin
 *
 * Created on May 31, 2019, 12:25 PM
 */


#include "GPGOMEA/Fitness/PythonFitness.h"

using namespace std;
using namespace arma;
namespace py = boost::python;
namespace np = boost::python::numpy;

PythonFitness::PythonFitness() {
    Py_Initialize();
    np::initialize();
    PyRun_SimpleString("import sys; sys.path.insert(0,'./')");
}

void PythonFitness::SetPythonCallableFunction(std::string filename, std::string function_name) {

    try {
        py::object module = py::import(py::str(filename));
        callable_python_function = py::object(module.attr(py::str(function_name)));
    } catch (boost::python::error_already_set const &) {
        std::string err = "PythonFitness::SetPythonCallableFunction error: perhaps misspelled names in pyprobdef?\n" + filename + " " + function_name;
        throw std::runtime_error(err);
    }

}

double_t PythonFitness::ComputeFitness(Node* n, bool use_caching) {

    evaluations++;

    vec out = n->GetOutput(this->TrainX, use_caching);
    np::ndarray npout = Utils::ToNumpyArray(out);

    //cout << py::str(callable_python_function) << endl;
    py::object pyresult = callable_python_function(npout);
    double_t fit = py::extract<double_t>(pyresult);

    if (std::isnan(fit)) {
        fit = arma::datum::inf;
    }
    n->cached_fitness = fit;

    return fit;
}
