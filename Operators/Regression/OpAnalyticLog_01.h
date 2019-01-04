/*
 


 */

/* 
 * File:   OpLog.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:37 PM
 */

#ifndef OPANALYTICLOG_01_H
#define OPANALYTICLOG_01_H

#include "../Operator.h"

class OpAnalyticLog01 : public Operator{
public:
    
    OpAnalyticLog01() {
        arity = 1;
        name = "alog0.01";
        type = OperatorType::opFunction;
    }
    
    Operator * Clone() const override {
        return new OpAnalyticLog01(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        arma::vec res = arma::log( arma::sqrt( 0.01 + arma::square(x.col(0))) );
        return res;
    }
    
    arma::vec Invert(const arma::vec& desired_elem, const arma::vec& output_siblings, size_t idx) override {
        throw std::runtime_error("OpAnalyticLog01::Invert - Not implemented");
        //return arma::join_cols( arma::exp(desired_elem), -1 * arma::exp(desired_elem) );
    }

    
    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return name + "("+args[0]+")";
    }
    
    arma::vec ComputeDerivative( const arma::mat & x, const arma::mat & dx ) override {
        return x.col(0) / (0.01 + arma::square(x.col(0))) % dx.col(0);
    }
   
private:
    
};

#endif /* OPANALYTICLOG_01_H */

