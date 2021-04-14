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

#include "GPGOMEA/Operators/Operator.h"

class OpNewProtectedDivision : public Operator {
public:

    OpNewProtectedDivision() {
        arity = 2;
        name = "p/";
        type = OperatorType::opFunction;
        is_arithmetic = true;
    }

    Operator* Clone() const override {
        return new OpNewProtectedDivision(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec b = x.col(1);
        arma::vec protb = 1e-6 + arma::abs(b);
        arma::vec signb = arma::ones(b.n_elem);
        
        for(size_t i=0; i < b.n_elem; i++)
          if (b[i] < 0)
             signb[i] = -1;
     
        return signb % ( x.col(0) / protb );
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }
    
private:

};

#endif /* OPNEWPROTECTEDDIVISION_H */

