/*
 


 */

/* 
 * File:   TournamentSelection.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 3:04 PM
 */

#include "GPGOMEA/Selection/TournamentSelection.h"

using namespace std;
using namespace arma;


Node * TournamentSelection::GetTournamentSelectionWinner(const std::vector<Node*>& candidates, size_t tournament_size) {
    Node * winner = candidates[ arma::randu() * candidates.size() ];
    double_t winner_fit = winner->cached_fitness;

    for (size_t i = 1; i < tournament_size; i++) {
        Node * candidate = candidates[ arma::randu() * candidates.size() ];
        if (candidate->cached_fitness < winner_fit) {
            winner = candidate;
            winner_fit = candidate->cached_fitness;
        }
    }
    
    return winner;
}

Node * TournamentSelection::GetMOTournamentSelectionWinner(const std::vector<Node*>& candidates, size_t tournament_size) {
    Node * winner = candidates[ arma::randu() * candidates.size() ];

    for (size_t i = 1; i < tournament_size; i++) {
        Node * candidate = candidates[ arma::randu() * candidates.size() ];
        if ((candidate->rank < winner->rank) || 
            (candidate->rank == winner->rank && candidate->crowding_distance > winner->crowding_distance) ) {
            winner = candidate;
        }
    }
    
    return winner;
}

vector<Node*> TournamentSelection::PopulationWiseTournamentSelection(const std::vector<Node*> population, size_t selection_size, size_t tournament_size) {
    size_t n_pop = population.size();
    vector<Node*> selected;
    selected.reserve(selection_size);

    // make sure that n_pop is multiple of tournament_size
    assert(  ((double)n_pop) / tournament_size == (double) n_pop / tournament_size );

    size_t n_selected_per_round = n_pop / tournament_size;
    size_t n_rounds = selection_size / n_selected_per_round;

    for(size_t i = 0; i < n_rounds; i++){
        // get a random permutation 
        auto perm = arma::randperm(n_pop);

        // apply tournaments
        for(size_t j = 0; j < n_selected_per_round; j++) {
            // one tournament instance
            Node * winner = population[perm[j*tournament_size]];
            for(size_t k=j*tournament_size + 1; k < (j+1)*tournament_size; k++){
                if (population[perm[k]]->cached_fitness < winner->cached_fitness) {
                    winner = population[perm[k]];
                }
            }
            selected.push_back(winner);
        }
    }
    return selected;
}