/*
 


 */

/* 
 * File:   OpAnalyticQuotient.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPANALYTICQUOTIENT_H
#define OPANALYTICQUOTIENT_H

#include "GPGOMEA/Operators/Operator.h"

class OpAnalyticQuotient : public Operator {
public:

    OpAnalyticQuotient() {
        arity = 2;
        name = "aq";
        type = OperatorType::opFunction;
    }

    Operator* Clone() const override {
        return new OpAnalyticQuotient(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        return x.col(0) / arma::sqrt(1.0 + arma::square(x.col(1)));
    }

    arma::vec Invert(const arma::vec & desired_elem, const arma::vec& output_siblings, size_t idx) override {
        arma::vec res;
        
        if (idx == 0) {
            // a = y * sqrt( 1 + b^2 )
            res = desired_elem * sqrt(1.0 + output_siblings[0] * output_siblings[0]);
            
        } else {
            // b = sqrt( (a/y)^2 - 1 )
            // remove all unfeasible values
            std::vector<double_t> res_v;
            res_v.reserve(desired_elem.n_elem);
            
            double_t oo = output_siblings[0] * output_siblings[0];
            
            for (size_t i = 0; i < desired_elem.n_elem; i++) {
                double_t v = desired_elem[i];
                if (v == 0)
                    continue;
                double_t r = oo / (v * v) - 1.0;
                if (r < 0 || std::isinf(r))
                    continue;

                r = std::sqrt(r);
                res_v.push_back(r);
            }

            if (res_v.empty()) {
                
                res = arma::vec(1);
                res[0] = arma::datum::inf;
                
            } else {
                
                res = arma::vec(res_v.size());
                size_t i = 0;
                for (double_t v : res_v) {
                    res[i] = v;
                    i++;
                }
                
            }
        }


        return res;
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return "(" + args[0] + name + args[1] + ")";
    }
    
private:

};

#endif /* OPANALYTICQUOTIENT_H */

