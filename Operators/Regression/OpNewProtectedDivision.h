/*
 


 */

/* 
 * File:   OpNewProtectedDivision.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPNEWPROTECTEDDIVISION_H
#define OPNEWPROTECTEDDIVISION_H

#include "../Operator.h"

class OpNewProtectedDivision : public Operator {
public:

    OpNewProtectedDivision() {
        arity = 2;
        name = "p/";
        type = OperatorType::opFunction;
    }

    Operator* Clone() const override {
        return new OpNewProtectedDivision(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec absb = arma::abs(x.col(1));
        return x.col(1)/absb % ( x.col(0) / (1e-3 + absb) );
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }
    
private:

};

#endif /* OPNEWPROTECTEDDIVISION_H */

