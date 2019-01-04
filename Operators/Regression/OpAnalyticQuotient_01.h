/*
 


 */

/* 
 * File:   OpAnalyticQuotient.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPANALYTICQUOTIENT_01_H
#define OPANALYTICQUOTIENT_01_H

#include "../Operator.h"

class OpAnalyticQuotient01 : public Operator {
public:

    OpAnalyticQuotient01() {
        arity = 2;
        name = "aq0.1";
        type = OperatorType::opFunction;
    }

    Operator* Clone() const override {
        return new OpAnalyticQuotient01(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        return x.col(0) / arma::sqrt(0.1 + arma::square(x.col(1)));
    }

    arma::vec Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) override {
        arma::vec res;
        if (idx == 0) {
            // a = y * sqrt( 1 + b^2 )
            res = desired_elem * sqrt(0.1 + output_siblings[0] * output_siblings[0]);
            
        } else {
            // b = sqrt( (a/y)^2 - 1 )
            // remove all unfeasible values
            std::vector<double_t> res_v;
            res_v.reserve(desired_elem.n_elem);
            
            double_t oo = output_siblings[0] * output_siblings[0];
            
            for (double_t v : desired_elem) {
                if (v == 0)
                    continue;
                double_t r = oo / (v * v) - 0.1;
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
        return "(" + args[0] + "/ sqrt( 0.1 + (" + args[1] + ")^2 ) ) ";
    }
    
    arma::vec ComputeDerivative( const arma::mat & x, const arma::mat & dx ) override {
        return dx.col(0) % x.col(1) + x.col(0) % ( - x.col(1) / arma::pow( 0.1 + arma::square(x.col(1)), 1.5 ) ) % dx.col(1);
    }

private:

};

#endif /* OPANALYTICQUOTIENT_01_H */

