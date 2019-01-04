/*
 


 */

/* 
 * File:   TestsSemantics.h
 * Author: marco
 *
 * Created on July 1, 2018, 10:10 PM
 */

#ifndef TESTSSEMANTICBACKPROPAGATION_H
#define TESTSSEMANTICBACKPROPAGATION_H

#include "../Operators/Regression/OpPlus.h"
#include "../Operators/Regression/OpMinus.h"
#include "../Operators/Regression/OpTimes.h"
#include "../Operators/Regression/OpAnalyticQuotient.h"
#include "../Operators/Regression/OpExp.h"
#include "../Operators/Regression/OpLog.h"
#include "../Operators/Regression/OpSin.h"
#include "../Operators/Regression/OpCos.h"
#include "../Operators/Regression/OpSquare.h"
#include "../Operators/Regression/OpSquareRoot.h"

#include "../Fitness/SymbolicRegressionFitness.h"
#include "../Semantics/SemanticLibrary.h"

#include "../Variation/TreeInitializer.h"

#include <iostream>
#include <armadillo>
#include <assert.h>

using namespace std;
using namespace arma;

static void TestInversions() {

    vec d(1);
    d[0] = 3;
    vec o(1);
    o[0] = 2;

    vec res;

    // Plus
    Operator * op = new OpPlus();
    res = op->Invert(d, o, 0);

    assert(res[0] == d[0] - o[0]);

    delete op;

    // Minus
    op = new OpMinus();
    // a = y + b;
    res = op->Invert(d, o, 0);

    assert(res[0] == d[0] + o[0]);

    // b = a - y;
    res = op->Invert(d, o, 1);

    assert(res[0] == o[0] - d[0]);

    delete op;

    // Times
    op = new OpTimes();
    res = op->Invert(d, o, 0);

    assert(res[0] == d[0] / o[0]);

    d[0] = 0;
    o[0] = 0;
    res = op->Invert(d, o, 0);

    assert(isnan(res[0]));

    delete op;

    // AnalyticQuotient
    d[0] = 3;
    o[0] = 7;
    op = new OpAnalyticQuotient();
    res = op->Invert(d, o, 0);

    assert(res.n_elem == 1);
    assert(res[0] == d[0] * sqrt(1 + o[0] * o[0]));

    // b = sqrt( (a/y)^2 - 1 )
    res = op->Invert(d, o, 1);

    assert(res[0] == sqrt(o[0] / d[0] * o[0] / d[0] - 1.0));

    o[0] = -1;
    res = op->Invert(d, o, 1);

    assert(res[0] == datum::inf);

    d = vec(2);
    d[0] = -1;
    d[1] = 2;
    res = op->Invert(d, o, 1);

    assert(res.n_elem == 1);
    assert(res[0] == 0);

    delete op;

}

static void TestLibrary() {

    Fitness fit = SymbolicRegressionFitness();
    fit.TrainX = arma::mat(10, 10, fill::randu);
    TreeInitializer tree_init(TreeInitType::TreeInitHH);


    vector<Operator*> functions = {new OpAnalyticQuotient(), new OpPlus(), new OpMinus(), new OpTimes()};
    vector<Operator*> terminals = {new OpVariable(0), new OpVariable(1)};


    SemanticLibrary sl;
    sl.GenerateRandomLibrary(6, 50, fit, functions, terminals, tree_init, true);
    
    vec x(fit.TrainX.n_rows, fill::randu);
    
    pair<Node *, double_t> best_n_dist = sl.GetClosestSubtree(x, 4);
    
    assert(best_n_dist.first->GetHeight(false) <= 4);
    assert(!std::isinf(best_n_dist.second));
    
    cout << best_n_dist.first->GetSubtreeExpression(true) << endl;
    cout << best_n_dist.second << endl;

    for (Operator * op : functions)
        delete op;
    for (Operator * op : terminals)
        delete op;

}

#endif /* TESTSSEMANTICBACKPROPAGATION_H */

