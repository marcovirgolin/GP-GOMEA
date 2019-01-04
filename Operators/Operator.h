/*
 


 */

/* 
 * File:   Operator.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:19 PM
 */

#ifndef OPERATOR_H
#define OPERATOR_H

#include "../Utils/Exceptions.h"

#include <string>
#include <armadillo>
#include <set>

enum OperatorType {
    opFunction, opTerminal
};

enum OperatorTerminalType {
    opTermVariable, opTermConstant    
};

class Operator {
public:

    Operator();
    Operator(size_t id);
    //Operator(const Operator& orig);

    virtual Operator * Clone() const;
    virtual ~Operator();

    virtual arma::vec ComputeDerivative(const arma::mat & x, const arma::mat & dx);

    virtual arma::vec ComputeOutput(const arma::mat & x);

    arma::vec InvertAndPostproc(const arma::vec & desired_elem, const arma::vec & output_siblings, size_t idx);

    virtual arma::vec Invert(const arma::vec & desired_elem, const arma::vec & output_siblings, size_t idx);

    virtual std::string GetHumanReadableExpression(const std::vector<std::string> & args);


    size_t arity;
    OperatorType type;
    OperatorTerminalType term_type;
    std::string name;
    size_t id;

private:

};

#endif /* OPERATOR_H */

