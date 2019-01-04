/* 
 * File:   PopulationInitializer.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 11:32 AM
 */

#include "PopulationInitializer.h"

using namespace arma;
using namespace std;

std::vector<Node*> PopulationInitializer::InitializeTreePopulation(const ConfigurationOptions & conf, const TreeInitializer & tree_initializer, const Fitness & fitness) {

    std::vector<Node *> pop;
    pop.reserve(conf.population_size);

    size_t init_tries = 0;

    TreeInitShape tis = TreeInitShape::TreeInitFULL;
    for (size_t i = 0; i < conf.population_size; i++) {

        if (i > conf.population_size / 2)
            tis = TreeInitShape::TreeInitGROW;

        Node * n = tree_initializer.InitializeRandomTree(tis, conf.initial_maximum_tree_height, conf.functions, conf.terminals);

        // check for uniqueness
        if (conf.semantic_uniqueness_tries > 0 && init_tries < conf.semantic_uniqueness_tries) {
            n = EnsureSemanticUniqueness(n, pop, conf, tree_initializer, conf.initial_maximum_tree_height, init_tries, fitness);
            if (init_tries == conf.semantic_uniqueness_tries)
                cout << " % warning: maximum number of tries for syntactic unique initialization reached " << endl;
        } else if (conf.syntactic_uniqueness_tries > 0 && init_tries < conf.syntactic_uniqueness_tries) {
            n = EnsureSyntacticUniqueness(n, pop, conf, tree_initializer, conf.initial_maximum_tree_height, init_tries);
            if (init_tries == conf.syntactic_uniqueness_tries)
                cout << " % warning: maximum number of tries for syntactic unique initialization reached " << endl;
        }

        pop.push_back(n);
    }

    return pop;
}

Node * PopulationInitializer::EnsureSyntacticUniqueness(Node* n, const std::vector<Node*> & pop, const ConfigurationOptions & conf, const TreeInitializer & tree_initializer, size_t height, size_t & tries) {

    do {
        vector<Node*> o_nodes;
        vector<Node*> n_nodes = n->GetSubtreeNodes(true);
        bool identical_found = false;

        for (Node * o : pop) {
            o_nodes = o->GetSubtreeNodes(true);
            if (o_nodes.size() == n_nodes.size()) {
                for (size_t k = 0; k < o_nodes.size(); k++) {
                    if (!o_nodes[k]->GetValue().compare(n_nodes[k]->GetValue()))
                        break;
                    identical_found = true;
                }
            }
        }

        if (!identical_found)
            break;

        // identical found, retry
        n->ClearSubtree();
        n = tree_initializer.InitializeRandomTree(TreeInitShape::TreeInitFULL, height, conf.functions, conf.terminals);
        tries++;

    } while (tries < conf.syntactic_uniqueness_tries);

    return n;

}

Node * PopulationInitializer::EnsureSemanticUniqueness(Node* n, const std::vector<Node*> & pop, const ConfigurationOptions & conf, const TreeInitializer & tree_initializer, size_t height, size_t& tries, const Fitness & fitness) {

    do {

        vec o_output;
        vec n_output = n->GetOutput(fitness.TrainX, conf.caching);
        bool identical_found = false;
        for (Node * o : pop) {
            o_output = o->GetOutput(fitness.TrainX, conf.caching);

            for (size_t i = 0; i < n_output.n_elem; i++) {
                if (o_output[i] != n_output[i]) {
                    break;
                }
                identical_found = true;
            }

            if (identical_found)
                break;
        }

        if (!identical_found)
            break;

        // identical found, retry
        n->ClearSubtree();
        n = tree_initializer.InitializeRandomTree(TreeInitShape::TreeInitFULL, height, conf.functions, conf.terminals);
        tries++;

    } while (tries < conf.semantic_uniqueness_tries);

    return n;
}
