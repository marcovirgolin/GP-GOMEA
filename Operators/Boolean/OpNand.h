/*
 


 */

/* 
 * File:   OpPlus.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPNAND_H
#define OPNAND_H

#include "../Operator.h"

class OpNand : public Operator {
public:

    OpNand() {
        arity = 2;
        name = "NAND";
        type = OperatorType::opFunction;
    }

    Operator * Clone() const override {
        return new OpNand(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec res(x.n_rows);

        for (int i = 0; i < x.n_rows; i++)
            res[i] = !((bool) x(0, i) & (bool) x(1, i));

        return res;
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }


private:

};

#endif /* OPNAND_H */

