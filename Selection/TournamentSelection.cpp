/*
 


 */

/* 
 * File:   TournamentSelection.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 3:04 PM
 */

#include "TournamentSelection.h"

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
