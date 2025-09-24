/*
 


 */

/* 
 * File:   Operator.cpp
 * Author: virgolin
 * 
 * Created on June 27, 2018, 12:19 PM
 */

#include "GPGOMEA/Operators/Operator.h"

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
    return res;
}
