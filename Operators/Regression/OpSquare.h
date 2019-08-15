/*
 


 */

/* 
 * File:   OpSquare.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPSQUARE_H
#define OPSQUARE_H

#include "../Operator.h"

class OpSquare : public Operator {
public:

    OpSquare() {
        arity = 1;
        name = "^2";
        type = OperatorType::opFunction;
    }

    Operator * Clone() const override {
        return new OpSquare(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        return arma::square(x.col(0));
    }

    arma::vec Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) override {
        // using normal vector first as it's more efficient
        std::vector<double_t> res_v;
        arma::vec res;
        res_v.reserve(desired_elem.n_elem);
        double_t v;
        for (size_t i = 0; i < desired_elem.n_elem; i++) {
            v = desired_elem[i];
            if (v >= 0) {
                res_v.push_back(sqrt(v));
            }
        }

        if (res_v.empty()) {
            // impossibility
            res = arma::vec(1);
            res[0] = arma::datum::inf;
        } else {
            res = arma::vec(res_v.size());
            for (size_t i = 0; i < res_v.size(); i++)
                res[i] = res_v[i];
        }
        
        return res;

    }
    
    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "("+args[0]+")"+name;
    }
    

private:

};

#endif /* OPPLUS_H */

