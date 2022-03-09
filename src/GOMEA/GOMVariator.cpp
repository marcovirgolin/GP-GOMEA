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

Node* GOMVariator::GOM(const Node& sol, const std::vector<Node*> & donors, const std::vector<std::vector<size_t> >& FOS, Fitness & fit, 
    double_t coeff_mut_prob, double_t coeff_mut_strength, GOMCoeffMutStrat gom_coeff_mut_strat, 
    bool use_caching) {

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
            // no need to make checks if off_node == don_node
            // but if it is a coefficient and it might mutate, then we should back it up 
            if (off_node->GetValue().compare(don_node->GetValue()) != 0 ||
                (gom_coeff_mut_strat==GOMCoeffMutStrat::gcmsWithin && coeff_mut_prob != 0 && don_node->op->type == OperatorType::opTermConstant) 
                ) {
                // gotta backup
                Operator * replaced_op = off_node->ChangeOperator(*don_node->op);
                backup_operators.push_back(replaced_op);
                changed_indices.push_back(j);
            } else {
                // no need
                continue;
            }
        }

        if (coeff_mut_prob != 0 && gom_coeff_mut_strat==GOMCoeffMutStrat::gcmsWithin) {
            // collect coeffs
            size_t num_changed_indices = changed_indices.size();
            vector<Node*> coeffs;
            coeffs.reserve(num_changed_indices);
            for(size_t j : changed_indices) {
                off_node = offspring_nodes[j];
                if (off_node->op->type == OperatorType::opTermConstant) {
                    coeffs.push_back(off_node);
                }
            }

            // Mutate constants
            SubtreeVariator::RandomCoefficientMutation(coeffs, coeff_mut_prob, coeff_mut_strength, use_caching);

            // double-check that these have actually changed, because e.g.
            // the donor might have had the same constant and  
            // mutation was actually not applied
            vector<size_t> pos_of_indices_that_actually_did_not_change;
            pos_of_indices_that_actually_did_not_change.reserve(num_changed_indices);
            for(size_t j=0; j < num_changed_indices; j++) {
                off_node = offspring_nodes[ changed_indices[j] ];
                if (off_node->op->type != OperatorType::opTermConstant
                    || backup_operators[j]->type != OperatorType::opTermConstant) {
                    continue;
                }
                if (((OpRegrConstant*) backup_operators[j])->GetConstant() == ((OpRegrConstant*)off_node->op)->GetConstant()){
                    pos_of_indices_that_actually_did_not_change.push_back(j);
                }
            }

            // remove indices that did not change from changed indices and clean up backup_operators
            for(int j = pos_of_indices_that_actually_did_not_change.size() - 1; j > -1; j--) {
                size_t pos = pos_of_indices_that_actually_did_not_change[j];
                changed_indices.erase(changed_indices.begin() + pos);
                delete backup_operators[pos];
                backup_operators.erase(backup_operators.begin() + pos);
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

        // if interleaved coeff mut
        if (coeff_mut_prob != 0 && gom_coeff_mut_strat == GOMCoeffMutStrat::gcmsInterleaved){
            GOMVariator::CoeffMutGOMStyle(offspring, 1, fit, coeff_mut_prob, coeff_mut_strength, use_caching);
        }
    }

    return offspring;
}


void GOMVariator::CoeffMutGOMStyle(Node * sol, size_t num_of_attempts, Fitness & fit, double_t coeff_mut_prob, double_t coeff_mut_strength, bool use_caching) {

    double_t backup_fit = sol->cached_fitness;
    vector<Node*> nodes = sol->GetSubtreeNodes(false);
    vector<Node*> const_nodes;
    vector<double_t> const_values_before;
    vector<bool> which_are_active;
    const_nodes.reserve(nodes.size()/2+1);
    const_values_before.reserve(nodes.size()/2+1);
    which_are_active.reserve(nodes.size()/2+1);
    for(size_t j = 0; j < nodes.size(); j++) {
        Node * n = nodes[j];
        if (n->op->type == OperatorType::opTermConstant) {
            const_nodes.push_back(n);
            const_values_before.push_back(((OpRegrConstant*)n->op)->GetConstant());
            which_are_active.push_back(n->IsActive());
        }
    }

    if (const_values_before.empty()) {
        return;
    }

    for(size_t j = 0; j < num_of_attempts; j++) {
        SubtreeVariator::RandomCoefficientMutation(const_nodes, coeff_mut_prob, coeff_mut_strength, use_caching);
        // check whether we actually need a fitness eval
        // to do that, first check if at least one active const changed
        bool gotta_eval = false;
        for(size_t k =0; k < const_values_before.size(); k++) {
            if (which_are_active[k]) {
                // check that the value changed
                double_t val_before = const_values_before[k];
                double_t val_new = ((OpRegrConstant*)const_nodes[k]->op)->GetConstant();
                if (val_new != val_before) {
                    gotta_eval = true;
                    break;
                }
            }
        }
        bool accept_change = true;
        if (gotta_eval){
            double_t new_fit = fit.ComputeFitness(sol, use_caching);
            if (new_fit > backup_fit) {
                accept_change = false;
            }
        }
        if (accept_change) {
            // update backup info
            for (size_t k = 0; k < const_values_before.size(); k++) {
                const_values_before[k] = ((OpRegrConstant*)const_nodes[k]->op)->GetConstant();
            }
            backup_fit = sol->cached_fitness;
            // note: active nodes and node positions do not change
        } else {
            // reset offspring 
            for (size_t k = 0; k < const_values_before.size(); k++) {
                ((OpRegrConstant*)const_nodes[k]->op)->SetConstant(const_values_before[k]);
                if (use_caching){
                    const_nodes[k]->ClearCachedOutput(true);
                }
            }
            sol->cached_fitness = backup_fit;
        }
    }
}

/*
Node* GOMVariator::GOM(const Node& sol, const std::vector<Operator*>& functions, const std::vector<Operator*>& terminals, const std::vector<std::vector<size_t> >& FOS, Fitness& fit, 
    double_t coeff_mut_prob, double_t coeff_mut_strength,
    bool use_caching) {

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
*/


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
