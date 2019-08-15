/*
 


 */

/* 
 * File:   Node.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 11:57 AM
 */

#ifndef NODE_H
#define NODE_H

#include "../Utils/Exceptions.h"
#include "../Operators/Operator.h"

#include <vector>
#include <armadillo>
#include <iostream>
#include <assert.h>
#include <queue>

enum SubtreeParseType {
    SubtreeParsePREORDER, SubtreeParsePOSTORDER, SubtreeParseLEVELORDER
};

class Node {
public:

    Node(Operator * op);
    Node(const Node& orig);
    virtual ~Node();

    arma::vec GetOutput(const arma::mat & x, bool use_caching);

    void ClearCachedOutput(bool also_ancestors);

    Node * CloneSubtree() const;

    std::string GetSubtreeExpression(bool only_active);

    std::string GetSubtreeHumanExpression();

    size_t GetArity() const;

    std::string GetValue() const;

    void AppendChild(Node * c);

    std::vector<Node *>::iterator DetachChild(Node * c);

    Node * DetachChild(size_t index);

    void InsertChild(Node * c, std::vector<Node *>::iterator);

    std::vector<Node *> GetSubtreeNodes(bool only_active, SubtreeParseType pt = SubtreeParseType::SubtreeParsePREORDER);

    Operator * ChangeOperator(const Operator & other);

    size_t GetHeight(bool only_active = true) const;
    size_t GetDepth() const;

    size_t GetRelativeIndex();

    bool IsActive();

    std::vector<arma::vec> GetDesiredOutput(const std::vector<arma::vec> & Y, const arma::mat & X, bool use_caching);

    void ClearSubtree();


    Node * parent;
    std::vector<Node *> children;

    Operator * op;
    arma::vec cached_output;

    double_t cached_fitness = arma::datum::inf;


private:
    void ComputeHeightRecursive(size_t & h, bool only_active) const;

    void GetSubtreeNodesPreorderRecursive(std::vector<Node*> & v, bool only_active);
    void GetSubtreeNodesPostorderRecursive(std::vector<Node*> & v, bool only_active);
    void GetSubtreeNodesLevelOrder(std::vector<Node*> & v,bool only_active);
    void GetSubtreeExpressionRecursive(std::string & expr, bool only_active);
    void GetSubtreeHumanExpressionRecursive(std::string & expr);
};



#endif /* NODE_H */

