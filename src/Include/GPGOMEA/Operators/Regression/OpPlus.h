/*
 


 */

/* 
 * File:   OpPlus.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPPLUS_H
#define OPPLUS_H

#include "GPGOMEA/Operators/Operator.h"

class OpPlus : public Operator{
public:
    
    OpPlus() {
        arity = 2;
        name = "+";
        type = OperatorType::opFunction;
        is_arithmetic = true;
    }
    
    Operator * Clone() const override {
        return new OpPlus(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        return x.col(0) + x.col(1);
    }
    
    arma::vec Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) override {
        return desired_elem - output_siblings[0];
    }
    
    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }
    
private:
    
};

#endif /* OPPLUS_H */

