/*
 


 */

/* 
 * File:   OpExp.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPEXP_H
#define OPEXP_H

#include "GPGOMEA/Operators/Operator.h"

class OpExp : public Operator {
public:

    OpExp() {
        arity = 1;
        name = "exp";
        type = OperatorType::opFunction;
        is_arithmetic = false;
    }

    Operator * Clone() const override {
        return new OpExp(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        return arma::exp(x.col(0));
    }

    arma::vec Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) override {
        std::vector<double_t> res_v;
        res_v.reserve(desired_elem.n_elem);
        for (double_t v : desired_elem) {
            if (v > 0){
                res_v.push_back(std::log(v));
            }
        }
        
        arma::vec res;
        if (res_v.empty()) {
            res = arma::vec(1);
            res[0] = arma::datum::inf;
        } else {
            res = arma::vec(res_v.size());
            for(size_t i = 0; i < res_v.size(); i++)
                res[i] = res_v[i];
        }
        return res;

    }
    
    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return name + "("+args[0]+")";
    }
    

private:

};

#endif /* OPEXP_H */

