/*
 


 */

/* 
 * File:   OpCos.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPCOS_H
#define OPCOS_H

#include "../Operator.h"

class OpCos : public Operator{
public:
    
    OpCos() {
        arity = 1;
        name = "cos";
        type = OperatorType::opFunction;
    }
    
    Operator * Clone() const override {
        return new OpCos(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        return arma::cos ( x.col(0) );
    }
    
    arma::vec Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) override {
        
        // Valid elements are only within -1 and 1
        std::vector<double_t> res_v;
        res_v.reserve(desired_elem.n_elem);
        for (double_t v : desired_elem) {
            if (abs(v) <= 1.0){
                res_v.push_back(std::acos(v));
                res_v.push_back(std::acos(v)+2 * arma::datum::pi);
                res_v.push_back(std::acos(v)-2 * arma::datum::pi);
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
    
    arma::vec ComputeDerivative(const arma::mat& x, const arma::mat & dx) override {
        return -arma::sin(x.col(0)) % dx.col(0);
    }


private:
    
};

#endif /* OPCOS_H */

