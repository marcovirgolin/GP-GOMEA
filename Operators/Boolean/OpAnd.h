/*
 


 */

/* 
 * File:   OpPlus.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPAND_H
#define OPAND_H

#include "../Operator.h"

class OpAnd : public Operator {
public:

    OpAnd() {
        arity = 2;
        name = "AND";
        type = OperatorType::opFunction;
    }

    Operator * Clone() const override {
        return new OpAnd(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec res(x.n_rows);

        for (int i = 0; i < x.n_rows; i++)
            res[i] = (bool) x(0, i) & (bool) x(1, i);

        return res;
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }


private:

};

#endif /* OPAND_H */

