/*
 


 */

/* 
 * File:   Utils.h
 * Author: virgolin
 *
 * Created on June 28, 2018, 10:41 AM
 */

#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <string> 
#include <armadillo>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <boost/functional/hash.hpp>


#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/mem_fn.hpp>
#include <boost/shared_ptr.hpp>


class Utils {
public:

    static std::string ReplaceCharInString(const std::string original, char to_replace, char replacing);

    static std::string ToLowerCase(const std::string original);

    static std::vector<std::string> SplitStringByChar(const std::string original, char splitc);

    static arma::vec Normalize(const arma::vec & x);

    static std::pair<double_t, double_t> ComputeMeanStdEfficiently(const arma::vec & x);

    static std::pair<double_t, double_t> ComputeMeanStdEfficiently(const std::vector<double_t> & x);

    static size_t HashArmaVec(const arma::vec & x);

    static std::pair<double_t, double_t> ComputeLinearScalingTerms(const arma::vec & P, const arma::vec & Y, double_t * precomputed_mean_Y = NULL, arma::vec * precomputed_var_terms_Y = NULL,
            double_t * precomputed_mean_P = NULL, double_t * precomputed_var_terms_P = NULL, double_t * precomputed_denom_P = NULL);

    static double_t ComputeDistanceWithDontCares(const std::vector<arma::vec> & x, const arma::vec& y, arma::vec * new_x = NULL);

    static double_t ComputeDistance(const arma::vec & x, const arma::vec& y, bool linear_scaling = false, double_t * precomputed_mean_Y = NULL, arma::vec * precomputed_var_terms_Y = NULL,
            double_t * precomputed_mean_P = NULL, double_t * precomputed_var_terms_P = NULL, double_t * precomputed_denom_P = NULL);

    static bool IsNumber(const std::string s);

    template <class T>
    static boost::python::list ToPythonList(std::vector<T> vector);
    static boost::python::list ToPythonList(arma::vec vector);

    template <class T>
    static boost::python::numpy::ndarray ToNumpyArray(std::vector<T> vector);
    static boost::python::numpy::ndarray ToNumpyArray(arma::vec vector);

    static arma::mat ConvertNumpyToArma(boost::python::numpy::ndarray npX);
    
private:
    Utils();
    ;
};

#endif /* UTILS_H */

