/*
 


 */

/* 
 * File:   OpRegrConstant.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 4:18 PM
 */

#ifndef INTERNAL_STRATEGY_PARAMETERS
#define INTERNAL_STRATEGY_PARAMETER_TAU 1e-1
#define INTERNAL_STRATEGY_PARAMETER_EPS 1e-16
#endif

#ifndef OPREGRCONSTANT_H
#define OPREGRCONSTANT_H


#include "GPGOMEA/Operators/Operator.h"

#include <armadillo>
#include <math.h>

class OpRegrConstant : public Operator {
public:

    OpRegrConstant(double_t lower_b, double_t upper_b) {
        arity = 0;
        constant = arma::datum::nan;
        this->lower_b = lower_b;
        this->upper_b = upper_b;
        name = std::to_string(constant);
        type = OperatorType::opTermConstant;
        internal_strategy_parameter = arma::datum::nan;
        // term_type = OperatorTerminalType::opTermConstant;
    }

    OpRegrConstant(double_t value) {
        if (std::isinf(value)) {
            throw std::runtime_error("OpRegrConstant::OpRegrConstant cannot be initialized to INF");
        }
        if (std::isnan(value)) {
            throw std::runtime_error("OpRegrConstant::OpRegrConstant cannot be initialized to NAN");
        }
        arity = 0;
        name = std::to_string(value);
        type = OperatorType::opTermConstant;
        // term_type = OperatorTerminalType::opTermConstant;
        constant = value;
    }

    Operator * Clone() const override {
        return new OpRegrConstant(*this);
    }

    arma::vec ComputeOutput(const arma::mat& x) override {
        if (std::isnan(constant)) {
            // first utilization, set up constant (& internal strategy parameter in case it will be used)
            constant = round( (arma::randu() * (upper_b - lower_b) + lower_b) * 1e3 ) / 1e3;
            name = std::to_string(constant);
            internal_strategy_parameter = std::max( 
                std::exp(arma::randn()*INTERNAL_STRATEGY_PARAMETER_TAU), 
                INTERNAL_STRATEGY_PARAMETER_EPS );
        }

        arma::vec out = arma::ones(x.n_rows);
        return out * constant;
    }

    std::string GetHumanReadableExpression(const std::vector<std::string>& args) override {
        return name;
    }

    double_t GetConstant() {
        return constant;
    }

    void SetConstant(double_t new_value) {
        constant = new_value;
        name = std::to_string(constant);
    }

    double_t lower_b, upper_b;
    double_t internal_strategy_parameter;


protected:

    double_t constant;

private:


};

#endif /* OPREGRCONSTANT_H */

