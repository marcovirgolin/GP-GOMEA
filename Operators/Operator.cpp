/*
 


 */

/* 
 * File:   Operator.cpp
 * Author: virgolin
 * 
 * Created on June 27, 2018, 12:19 PM
 */

#include "Operator.h"

Operator::Operator() {

}

Operator::Operator(size_t id) {
    
}

/*Operator::Operator(const Operator& orig) {
    arity = orig.arity;
    id = orig.id;
    name = orig.name;
    type = orig.type;
    term_type = orig.term_type;
}*/

Operator::~Operator() {
}



Operator * Operator::Clone() const {
    throw NotImplementedException("Operator::Clone not implemented");
}

arma::vec Operator::ComputeOutput(const arma::mat& x) {
    throw NotImplementedException("Operator::ComputeOutput not implemented");
}

arma::vec Operator::ComputeDerivative( const arma::mat & x, const arma::mat & dx) {
    throw NotImplementedException("Operator::ComputeDerivative not implemented");
}

arma::vec Operator::Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) {
    throw NotImplementedException("Operator::Invert not implemented");
}

std::string Operator::GetHumanReadableExpression(const std::vector<std::string> & args ) {
    throw NotImplementedException("Operator::GetHumanReadableExpression not implemented");
}


arma::vec Operator::InvertAndPostproc(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) {
    arma::vec res = Invert(desired_elem, output_siblings, idx);
    if (std::isinf(res[0]) || std::isnan(res[0]))
        return res;

    /*std::set<double_t> res_s;
    double_t v;
    double_t av;
    for (size_t i = 0; i < res.size(); i++) {
        v = res[i];

        if (v > 1e100) {
            v = 1e100;
        } else if (v < -1e100) {
            v = -1e100;
        } else {
            av = abs(v);
            if (av < 1e-100)
                v = 0;
        }
        res_s.insert(v);
    }

    res = arma::vec(res_s.size());
    size_t i = 0;
    for (double_t v : res_s) {
        res[i] = v;
        i++;
    }*/

    return res;
}
