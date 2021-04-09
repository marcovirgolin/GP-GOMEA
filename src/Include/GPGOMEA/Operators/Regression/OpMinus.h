/*
 


 */

/* 
 * File:   OpMinus.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPMINUS_H
#define OPMINUS_H

#include "GPGOMEA/Operators/Operator.h"

#include <armadillo>

class OpMinus : public Operator {
public:

    OpMinus() {
        arity = 2;
        name = "-";
        type = OperatorType::opFunction;
    }
    
    Operator* Clone() const override {
        return new OpMinus(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        return x.col(0) - x.col(1);
    }
    
    arma::vec Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) override {
        if (idx == 0)
            return desired_elem - output_siblings[0];
        else
            return output_siblings[0] - desired_elem;
    }
    
    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }
    
    
private:

};

#endif /* OPMINUS_H */

