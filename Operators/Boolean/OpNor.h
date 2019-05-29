/*
 


 */

/* 
 * File:   OpPlus.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
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
            res[i] = !((bool) x(0, i) | (bool) x(1, i));

        return res;
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }


private:

};

#endif /* OPNOR_H */

