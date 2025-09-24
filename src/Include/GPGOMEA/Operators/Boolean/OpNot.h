/*
 


 */

/* 
 * File:   OpPlus.h
 * Author: virgolin
 *
 * Created on May 30, 2019, 12:37 PM
 */

#ifndef OPNOT_H
#define OPNOT_H

#include "GPGOMEA/Operators/Operator.h"

class OpNot : public Operator {
public:

    OpNot() {
        arity = 1;
        name = "NOT";
        type = OperatorType::opFunction;
    }

    Operator * Clone() const override {
        return new OpNot(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec res(x.n_rows);

        for (size_t i = 0; i < x.n_rows; i++)
            res[i] = !(bool) x.at(i, 0);

        return res;
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return name + "(" + args[0] + ")";
    }


private:

};

#endif /* OPNOT_H */

