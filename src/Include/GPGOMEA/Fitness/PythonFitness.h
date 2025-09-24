/*
 


 */

/* 
 * File:   PythonFitness.h
 * Author: virgolin
 *
 * Created on June 22, 2019, 12:25 PM
 */

#ifndef PYTHONFITNESS_H
#define PYTHONFITNESS_H

#include <map>
#include <armadillo>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/mem_fn.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include "GPGOMEA/Fitness/Fitness.h"
#include "GPGOMEA/Utils/Utils.h"



class PythonFitness : public Fitness {
public:

    PythonFitness();
    
    void SetPythonCallableFunction(std::string filename, std::string function_name);
    double_t ComputeFitness(Node * n, bool use_caching) override;
    
private:
    
    boost::python::object callable_python_function; 

};

#endif /* PYTHONFITNESS_H */

