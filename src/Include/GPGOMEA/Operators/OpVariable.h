/*
 


 */

/* 
 * File:   OpVariable.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:43 PM
 */

#ifndef OPVARIABLE_H
#define OPVARIABLE_H

#include "Operator.h"
#include "GPGOMEA/Utils/Exceptions.h"

#include <armadillo>
#include <string>

class OpVariable : public Operator {
public:

    OpVariable(size_t id) {
        arity = 0;
        type = OperatorType::opTermVariable;
        // term_type = OperatorTerminalType::opTermVariable;
        this->id = id;
        this->name = "x" + std::to_string(id);
    }

    Operator * Clone() const override {
        return new OpVariable(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec out = x.col(id);
        return out;
    }
    
    std::string GetHumanReadableExpression(const std::vector<std::string> & args) override {
        return this->name;
    }
    

private:
    OpVariable();
};

#endif /* OPVARIABLE_H */

