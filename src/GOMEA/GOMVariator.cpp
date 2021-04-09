/*
 


 */

/* 
 * File:   GOMVariator.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 6:07 PM
 */



#include "GPGOMEA/GOMEA/GOMVariator.h"
#include "GPGOMEA/GOMEA/GOMEAFOS.h"

using namespace std;
using namespace arma;

Node* GOMVariator::GOM(const Node& sol, const std::vector<Node*> & donors, const std::vector<std::vector<size_t> >& FOS, Fitness & fit, bool use_caching) {

    Node * offspring = sol.CloneSubtree();
    double_t back_fit = sol.cached_fitness;

    vector<size_t> permut_fos_indices;
    permut_fos_indices.reserve(FOS.size());
    for (size_t i = 0; i < FOS.size(); i++)
        permut_fos_indices.push_back(i);
    std::random_shuffle(permut_fos_indices.begin(), permut_fos_indices.end());
   
    vector<Node*> offspring_nodes = offspring->GetSubtreeNodes(false);
    vector<Node*> donor_nodes;
    for (size_t i = 0; i < FOS.size(); i++) {
        vector<size_t> F = FOS[permut_fos_indices[i]];
        donor_nodes = donors[randu() * donors.size()]->GetSubtreeNodes(false);
        bool meaningful_change = false;
        // change the nodes of the offspring with the ones of the donor
        Node * off_node, * don_node;
        vector<Operator*> backup_operators;
        backup_operators.reserve(F.size());
        vector<size_t> changed_indices;
        changed_indices.reserve(F.size());
        for (size_t j : F) {
            off_node = offspring_nodes[j];
            don_node = donor_nodes[j];
            if (off_node->GetValue().compare(don_node->GetValue()) == 0)
                continue;
            else {
                Operator * replaced_op = off_node->ChangeOperator(*don_node->op);
                backup_operators.push_back(replaced_op);
                changed_indices.push_back(j);
            }
        }

        // check for meaningful changes (done later because situation may change during iterations)
        for (size_t j : changed_indices) {
            Node * off_node = offspring_nodes[j];
            if (off_node->IsActive()) {
                meaningful_change = true;
                break;
            }
        }

        // clean the cache of node outputs (even in case of not meaningful changes, for the future)
        if (use_caching) {
            for (size_t j : changed_indices) {
                off_node = offspring_nodes[j];
                off_node->ClearCachedOutput(true);
            }
        }

        // determine whether to keep or not the changes
        bool keep_changes = true;
        if (meaningful_change) {
            // ensure fitness did not get worse
            if (keep_changes) {
                double_t new_fit = fit.ComputeFitness(offspring, use_caching);
                if (new_fit <= back_fit) {
                    keep_changes = true;
                } else {
                    keep_changes = false;
                }
            }
        }

        if (keep_changes) {
            // keep changes            
            back_fit = offspring->cached_fitness;
        } else {
            // revert
            for (size_t j = 0; j < changed_indices.size(); j++) {
                off_node = offspring_nodes[ changed_indices[j] ];
                Operator * op = off_node->ChangeOperator(*backup_operators[j]);
                delete op;
                if (use_caching)
                    off_node->ClearCachedOutput(true);
            }
            offspring->cached_fitness = back_fit;
        }

        // discard backup
        for (Operator * op : backup_operators)
            delete op;
    }

    return offspring;
}

Node* GOMVariator::GOM(const Node& sol, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals, const std::vector<std::vector<size_t> >& FOS, Fitness& fit, bool use_caching) {

    Node * offspring = sol.CloneSubtree();
    double_t back_fit = sol.cached_fitness;

    vector<size_t> permut_fos_indices;
    permut_fos_indices.reserve(FOS.size());
    for (size_t i = 0; i < FOS.size(); i++)
        permut_fos_indices.push_back(i);
    std::random_shuffle(permut_fos_indices.begin(), permut_fos_indices.end());

    vector<Node*> offspring_nodes = offspring->GetSubtreeNodes(false);
    

    for (size_t i = 0; i < FOS.size(); i++) {

        vector<size_t> F = FOS[permut_fos_indices[i]];

        bool meaningful_change = false;
        // change the nodes of the offspring with the ones of the donor
        Node * off_node;
        vector<Operator*> backup_operators;
        backup_operators.reserve(F.size());
        vector<size_t> changed_indices;
        changed_indices.reserve(F.size());
        for (size_t j : F) {
            off_node = offspring_nodes[j];
            size_t h = off_node->GetHeight(false);

            Operator * don_op;
            if (h > 0 && randu() < 0.5) {
                don_op = functions[randu() * functions.size()];
            } else {
                don_op = terminals[randu() * terminals.size()];
            }

            if (don_op->name.compare(off_node->GetValue()) == 0)
                continue;

            Operator * replaced_op = off_node->ChangeOperator(*don_op);
            backup_operators.push_back(replaced_op);
            changed_indices.push_back(j);
        }

        // check for meaningful changes (done later because situation may change during iterations)
        for (size_t j : changed_indices) {
            Node * off_node = offspring_nodes[j];
            if (off_node->IsActive()) {
                meaningful_change = true;
                break;
            }
        }

        // clean the cache of node outputs (even in case of not meaningful changes, for the future)
        if (use_caching) {
            for (size_t j : changed_indices) {
                off_node = offspring_nodes[j];
                off_node->ClearCachedOutput(true);
            }
        }

        // determine whether to keep or not the changes
        bool keep_changes = true;
        if (meaningful_change) {
            // check fitness it not worse
            if (keep_changes) {
                double_t new_fit = fit.ComputeFitness(offspring, use_caching);
                if (new_fit <= back_fit) {
                    keep_changes = true;
                } else {
                    keep_changes = false;
                }
            }
        }

        if (keep_changes) {
            // keep changes            
            back_fit = offspring->cached_fitness;
        } else {
            // revert
            for (size_t j = 0; j < changed_indices.size(); j++) {
                off_node = offspring_nodes[ changed_indices[j] ];
                Operator * op = off_node->ChangeOperator(*backup_operators[j]);
                delete op;
                if (use_caching)
                    off_node->ClearCachedOutput(true);
            }
            offspring->cached_fitness = back_fit;
        }

        // discard backup
        for (Operator * op : backup_operators)
            delete op;
    }

    return offspring;
}


Node* GOMVariator::MakeBiggerGOMEATree(const Node& original, size_t orig_height, size_t desired_height, size_t max_arity, const GOMEATreeInitializer & tree_init, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals) {

    assert(desired_height > orig_height);

    Node * bigger = original.CloneSubtree();

    vector<Node*> nodes = bigger->GetSubtreeNodes(false);

    int height_left = desired_height - orig_height - 1;

    for (Node * n : nodes) {
        // take leaves
        if (n->GetDepth() == orig_height) {
            // append stuff
            Node * to_append;
            for (size_t i = 0; i < max_arity; i++) {
                if (height_left == 0) {
                    to_append = new Node(terminals[randu() * terminals.size()]->Clone());
                } else {
                    to_append = tree_init.GenerateTreeGrow(height_left, height_left, 0, max_arity, functions, terminals);
                }
                n->AppendChild(to_append);
            }
        }
    }

    size_t n = bigger->GetSubtreeNodes(false).size();
    size_t m = pow(2, desired_height + 1) - 1;
    assert(n == m);

    return bigger;

}
