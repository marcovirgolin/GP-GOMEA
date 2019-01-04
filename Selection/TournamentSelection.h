/*
 


 */

/* 
 * File:   TournamentSelection.h
 * Author: virgolin
 *
 * Created on June 28, 2018, 3:04 PM
 */

#ifndef TOURNAMENTSELECTION_H
#define TOURNAMENTSELECTION_H

#include "../Genotype/Node.h"

#include <armadillo>

class TournamentSelection {
public:

    static Node * GetTournamentSelectionWinner(const std::vector<Node*> & candidates, size_t tournament_size);

private:
    TournamentSelection() {};

};

#endif /* TOURNAMENTSELECTION_H */

