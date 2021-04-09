/*
 


 */

/* 
 * File:   OpLog.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPLOG_H
#define OPLOG_H

#include "GPGOMEA/Operators/Operator.h"

class OpLog : public Operator {
public:

    OpLog() {
        arity = 1;
        name = "plog";
        type = OperatorType::opFunction;
    }

    Operator * Clone() const override {
        return new OpLog(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec res = arma::log(arma::abs(x.col(0)));
        arma::uvec nf = arma::find_nonfinite(res);
        for (size_t i : nf) {
            res[i] = 0;
        }
        return res;
    }

    arma::vec Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) override {
        std::vector<double_t> res_v;
        res_v.reserve(desired_elem.n_elem * 2);
        double_t v1, v2;
        for (size_t i = 0; i < desired_elem.n_elem; i++) {
            v1 = exp(desired_elem[i]);
            v2 = -exp(desired_elem[i]);
            
            if (!std::isinf(v1) && !std::isnan(v1))
                res_v.push_back(v1);
            if (!std::isinf(v2) && !std::isnan(v2))
                res_v.push_back(v2);
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
        return name + "(" + args[0] + ")";
    }


private:

};

#endif /* OPLOG_H */

