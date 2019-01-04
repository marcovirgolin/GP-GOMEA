/*
 


 */

/* 
 * File:   OpSquareRoot.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPSQUAREROOT_H
#define OPSQUAREROOT_H

#include "../Operator.h"

class OpSquareRoot : public Operator {
public:

    OpSquareRoot() {
        arity = 1;
        name = "sqrt";
        type = OperatorType::opFunction;
    }

    Operator * Clone() const override {
        return new OpSquareRoot(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        return arma::sqrt(arma::abs(x.col(0)));
    }

    arma::vec Invert(const arma::vec & desired_elem, const arma::vec& output_siblings, size_t idx) override {
        // actually, should return all combinations of +s and -s for each element in desired_elem...
        return arma::join_cols(arma::square(desired_elem),
                -1*arma::square(desired_elem));
    }
    
    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return name + "("+args[0]+")";
    }
    
    arma::vec ComputeDerivative(const arma::mat& x, const arma::mat & dx) override {
        return 0.5 / arma::sqrt( x.col(0) ) % dx.col(0);
    }

private:

};

#endif /* OPSQUAREROOT_H */

