/*
 


 */

/* 
 * File:   OpTimes.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPTIMES_H
#define OPTIMES_H

#include "GPGOMEA/Operators/Operator.h"

class OpTimes : public Operator {
public:

    OpTimes() {
        arity = 2;
        name = "*";
        type = OperatorType::opFunction;
    }

    Operator* Clone() const override {
        return new OpTimes(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        return x.col(0) % x.col(1);
    }

    arma::vec Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) override {

        // check if it does not matter or if it's unfeasible
        if (output_siblings[0] == 0) {
            bool doesnt_matter = false;
            for (double_t v : desired_elem) {
                if (v == 0) {
                    doesnt_matter = true;
                    break;
                }
            }
            arma::vec res(1);
            if (doesnt_matter) {
                res[0] = arma::datum::nan;
            } else {
                res[0] = arma::datum::inf;
            }
            return res;
        }

        return desired_elem / output_siblings[0];
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {

        return "(" + args[0] + name + args[1] + ")";
    }

private:

};

#endif /* OPTIMES_H */

