/*
 


 */

/* 
 * File:   OpPlus.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPXOR_H
#define OPXOR_H

#include "../Operator.h"

class OpXor : public Operator {
public:

    OpXor() {
        arity = 2;
        name = "XOR";
        type = OperatorType::opFunction;
    }

    Operator * Clone() const override {
        return new OpXor(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec res(x.n_rows);

        for (int i = 0; i < x.n_rows; i++)
            res[i] = ((bool) x(0, i) | (bool)x(1, i)) & !((bool)x(0, i) & (bool)x(1, i));

        return res;
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }


private:

};

#endif /* OPXOR_H */

