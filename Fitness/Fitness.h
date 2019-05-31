/*
 


 */

/* 
 * File:   Fitness.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 6:14 PM
 */

#ifndef FITNESS_H
#define FITNESS_H

#include "../Utils/Exceptions.h"
#include "../Genotype/Node.h"

#include <armadillo>
#include <string>

enum FitnessCasesType {
    FitnessCasesTRAIN, FitnessCasesTEST, FitnessCasesVALIDATION
};

class Fitness {
    
public:
    
    Fitness();
    Fitness(const Fitness& orig);
    virtual ~Fitness();

    arma::mat ReadFitnessCases(std::string filepath);

    virtual void SetFitnessCases(const arma::mat & X, FitnessCasesType fct);

    virtual double_t ComputeFitness(Node * n, bool use_caching);
    
    arma::vec GetPopulationFitness(const std::vector<Node *> & population, bool compute, bool use_caching); 
    
    Node * GetBest(const std::vector<Node*> & population, bool compute, bool use_caching);
    
    virtual double_t GetTestFit(Node * n);
    
    virtual double_t GetValidationFit(Node * n);
    
    arma::mat TrainX;
    arma::vec TrainY;
    arma::mat ValidationX;
    arma::vec ValidationY;
    arma::mat TestX;
    arma::mat TestY;
    
    double_t trainY_mean = arma::datum::nan;
    double_t trainY_std = arma::datum::nan;
    
    arma::vec var_comp_trainY;

    
    size_t evaluations = 0;

private:

};

#endif /* FITNESS_H */

