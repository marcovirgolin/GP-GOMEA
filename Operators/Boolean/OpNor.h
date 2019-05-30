/*
 


 */

/* 
 * File:   OpPlus.h
 * Author: virgolin
 *
 * Created on May 30, 2019, 12:37 PM
 */

#ifndef OPNOR_H
#define OPNOR_H

#include "../Operator.h"

class OpNor : public Operator {
public:

    OpNor() {
        arity = 2;
        name = "NOR";
        type = OperatorType::opFunction;
    }

    Operator * Clone() const override {
        return new OpNor(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec res(x.n_rows);

        for (int i = 0; i < x.n_rows; i++)
            res[i] = !((bool) x.at(i, 0) | (bool) x.at(i, 1));

        return res;
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }


private:

};

#endif /* OPNOR_H */

