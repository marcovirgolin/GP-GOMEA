/*
 


 */

/* 
 * File:   Node.cpp
 * Author: virgolin
 * 
 * Created on June 27, 2018, 11:57 AM
 */

#include "GPGOMEA/Genotype/Node.h"

using namespace std;
using namespace arma;

using child_iter = std::vector<Node *>::iterator;

Node::Node(Operator * op) {
    this->op = op;
    this->parent = NULL;
    this->cached_fitness = arma::datum::inf;
}

Node::Node(const Node& orig) {
    op = orig.op->Clone();
    parent = NULL;
    children.clear();
    cached_fitness = orig.cached_fitness;
    cached_objectives = orig.cached_objectives;
    cached_output = orig.cached_output;
    rank = orig.rank;
    crowding_distance = orig.crowding_distance;
}

Node::~Node() {
    delete this->op;
}

arma::vec Node::GetOutput(const arma::mat & x, bool use_caching) {

    vec out;
    if (use_caching && !cached_output.empty())
        return cached_output;

    if (op->type == OperatorType::opFunction) {
        size_t arity = this->GetArity();
        mat xx(x.n_rows, arity);
        for (size_t i = 0; i < arity; i++)
            xx.col(i) = children[i]->GetOutput(x, use_caching);
        out = op->ComputeOutput(xx);
    } else
        out = op->ComputeOutput(x);

    if (use_caching)
        cached_output = out;

    return out;
}

size_t Node::GetArity() const {
    return op->arity;
}

std::string Node::GetValue() const {
    return op->name;
}

void Node::ClearCachedOutput(bool also_ancestors) {
    cached_output.clear();
    if (also_ancestors && parent)
        parent->ClearCachedOutput(also_ancestors);
}

void Node::AppendChild(Node * c) {
    children.push_back(c);
    c->parent = this;
}

child_iter Node::DetachChild(Node* c) {
    auto it = children.begin();
    for (it; it < children.end(); it++) {
        if ((*it) == c) {
            break;
        }
    }
    assert(it != children.end());
    children.erase(it);
    c->parent = NULL;
    return it;
}

Node* Node::DetachChild(size_t index) {
    auto it = children.begin() + index;
    Node * c = children[index];
    children.erase(it);
    c->parent = NULL;
    return c;
}

void Node::InsertChild(Node* c, child_iter it) {
    children.insert(it, c);
    c->parent = this;
}

size_t Node::GetDepth() const {
    Node * p = parent;
    size_t depth = 0;
    while (p != NULL) {
        depth++;
        p = p->parent;
    }
    return depth;
}

size_t Node::GetHeight(bool only_active) const {
    size_t h = GetDepth();
    size_t max_h = h;
    ComputeHeightRecursive(max_h, only_active);

    return max_h - h;
}

Operator* Node::ChangeOperator(const Operator & other) {
    Operator * old = op;
    op = other.Clone();
    return old;
}

void Node::ComputeHeightRecursive(size_t & h, bool only_active) const {
    size_t arity;
    if (only_active)
        arity = this->GetArity();
    else
        arity = children.size();

    if (arity == 0) {
        size_t d = this->GetDepth();
        if (d > h)
            h = d;
    }

    for (size_t i = 0; i < arity; i++)
        children[i]->ComputeHeightRecursive(h, only_active);
}

std::vector<Node*> Node::GetSubtreeNodes(bool only_active, SubtreeParseType pt) {
    std::vector<Node *> subtree_nodes;
    if (pt == SubtreeParseType::SubtreeParsePREORDER)
        GetSubtreeNodesPreorderRecursive(subtree_nodes, only_active);
    else if (pt == SubtreeParseType::SubtreeParsePOSTORDER)
        GetSubtreeNodesPostorderRecursive(subtree_nodes, only_active);
    else if (pt == SubtreeParseType::SubtreeParseLEVELORDER) {
        GetSubtreeNodesLevelOrder(subtree_nodes, only_active);
    } else
        throw std::runtime_error("Node::GetSubtreeNodes unrecognized subtree parse type");
    return subtree_nodes;
}

void Node::GetSubtreeNodesPreorderRecursive(std::vector<Node*>& v, bool only_active) {
    v.push_back(this);
    if (only_active) {
        size_t arity = GetArity();
        for (size_t i = 0; i < arity; i++) {
            children[i]->GetSubtreeNodesPreorderRecursive(v, only_active);
        }
    } else {
        for (size_t i = 0; i < children.size(); i++) {
            children[i]->GetSubtreeNodesPreorderRecursive(v, only_active);
        }
    }
}

void Node::GetSubtreeNodesPostorderRecursive(std::vector<Node*>& v, bool only_active) {
    if (only_active) {
        size_t arity = GetArity();
        for (size_t i = 0; i < arity; i++) {
            children[i]->GetSubtreeNodesPreorderRecursive(v, only_active);
        }
    } else {
        for (size_t i = 0; i < children.size(); i++) {
            children[i]->GetSubtreeNodesPreorderRecursive(v, only_active);
        }
    }
    v.push_back(this);
}

void Node::GetSubtreeNodesLevelOrder(std::vector<Node*> & v,bool only_active) {
    
    queue<Node*> q;
    Node * curr;
    
    q.push(this);
    q.push(NULL);
    
    while(q.size() > 1) {
        curr = q.front();
        q.pop();
        if (!curr)
            q.push(NULL);
        else {
            v.push_back(curr);
            size_t to_consider = only_active ? curr->GetArity() : curr->children.size();
            for(size_t i = 0; i < to_consider; i++) {
                q.push(curr->children[i]);
            }
        }
    }
}

Node* Node::CloneSubtree() const {
    Node * new_node = new Node(*this);
    for (Node * c : children) {
        Node * new_child = c->CloneSubtree();
        new_node->AppendChild(new_child);
    }
    return new_node;
}

std::string Node::GetSubtreeExpression(bool only_active_nodes) {
    string expr = "";
    GetSubtreeExpressionRecursive(expr, only_active_nodes);
    return expr;

}

std::string Node::GetSubtreeHumanExpression() {
    string result = "";

    GetSubtreeHumanExpressionRecursive(result);

    return result;
}

void Node::GetSubtreeExpressionRecursive(string & expr, bool only_active_nodes) {
    expr += GetValue();
    if (only_active_nodes)
        for (size_t i = 0; i < GetArity(); i++)
            children[i]->GetSubtreeExpressionRecursive(expr, only_active_nodes);
    else
        for (Node * c : children)
            c->GetSubtreeExpressionRecursive(expr, only_active_nodes);
}

void Node::GetSubtreeHumanExpressionRecursive(std::string& expr) {
    size_t arity = GetArity();

    vector<string> args;
    args.reserve(arity);

    for (size_t i = 0; i < arity; i++) {
        children[i]->GetSubtreeHumanExpressionRecursive(expr);
        args.push_back(expr);
    }

    expr = op->GetHumanReadableExpression(args);
}

void Node::ClearSubtree() {
    std::vector<Node *> subtree_nodes = GetSubtreeNodes(false);
    for (size_t i = 1; i < subtree_nodes.size(); i++)
        delete subtree_nodes[i];
    delete this;
}

size_t Node::GetRelativeIndex() {
    if (parent == NULL)
        return 0;
    size_t i = 0;
    for (Node * c : parent->children) {
        if (c == this)
            return i;
        i++;
    }
    throw std::runtime_error("Node::GetRelativeIndex unreachable code reached.");
}

bool Node::IsActive() {
    Node * p = parent;
    if (p == NULL)
        return true;
    Node * n = this;
    while (p != NULL) {
        if (n->GetRelativeIndex() >= p->GetArity())
            return false;
        n = p;
        p = n->parent;
    }
    return true;
}

std::vector<arma::vec> Node::GetDesiredOutput(const std::vector<arma::vec> & Y, const arma::mat & X, bool use_caching) {

    // if it is the root, return
    if (!parent) {
        return Y;
    }

    size_t idx = GetRelativeIndex();
    int num_siblings = parent->GetArity() - 1;
    arma::mat output_siblings(X.n_rows, num_siblings);

    size_t j = 0;
    for (size_t i = 0; i < num_siblings + 1; i++) {
        Node * sibling = parent->children[j];
        if (i == idx) {
            continue;
        }
        vec sibling_output = sibling->GetOutput(X, use_caching);
        output_siblings.col(j) = sibling_output;
        j++;
    }

    vector<vec> inversion;
    inversion.reserve(Y.size());

    for (size_t i = 0; i < Y.size(); i++) {
        vec y = Y[i];

        // propagate impossibility or dont' care
        if (std::isinf(y[0]) || std::isnan(y[0])) {
            inversion.push_back(y);
            continue;
        }

        vec r;
        vec out_sib;
        if (num_siblings > 0) {
            out_sib = output_siblings.row(i);
            if (out_sib.has_nan()) { // can happen, e.g. the sibling is sin(exp(exp(100)));
                inversion.clear();
                r = vec(1);
                r[0] = arma::datum::inf;
                inversion.push_back(r);
                return inversion;
            }
        }

        r = parent->op->InvertAndPostproc(y, out_sib, idx);

        if (std::isinf(r[0])) {
            inversion.clear();
            inversion.push_back(r);
            return inversion;
        }

        inversion.push_back(r);
    }

    for (vec x : inversion) {
        assert(x.n_elem > 0);
    }

    return inversion;

}



bool Node::Dominates(Node * o) {
    bool strictly_better_somewhere = false;
    for(size_t i = 0; i < cached_objectives.n_elem; i++) {
        if (cached_objectives[i] < o->cached_objectives[i])
            strictly_better_somewhere = true;
        else if (cached_objectives[i] > o->cached_objectives[i])
            return false;
    }
    return strictly_better_somewhere;
}



int Node::Count_N_NaComp(int count) {
    if (count == -1)
        count = 0;
    if (!op->is_arithmetic) {
        count += 1;
        vector<int> count_args; count_args.reserve(10);
        for(Node * c : children) {
            count_args.push_back(c->Count_N_NaComp(count));
        }
        int max_count = 0;
        for(int arg : count_args) {
            if (arg > max_count)
                max_count = arg;
        }
        count = max_count;
    }
    else {
        if (op->type == OperatorType::opFunction) {
            vector<int> count_args; count_args.reserve(10);
            for(Node * c : children) {
                count_args.push_back(c->Count_N_NaComp(count));
            }
            int max_count = 0;
            for(int arg : count_args) {
                if (arg > max_count)
                    max_count = arg;
            }
            count = max_count;
        }
    }
    return count;
}