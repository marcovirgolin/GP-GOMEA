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

/*
# this is surely bugged and I did not even try to run it given that it's night :')
function popwise_tournament_selection(pop, fit_array, selection_size, t_size; selector=argmin)
    n_pop = length(pop)
    survivors = []

    # make sure that n_pop is multiple of t_size
    @assert isinteger(n_pop / t_size)

    n_survivors_per_round = n_pop ÷ t_size
    n_rounds = selection_size ÷ n_survivors_per_round

    for i ∈ 1:n_rounds
        # get a random ordering
        random_order = Random.randperm(n_pop)
        # we consider the fitness values in that random order
        reordered_fit_array = fit_array[random_order]
        # reshape the fitness values from an 1D array into a 2D matrix with rows of size t_size
        reordered_fit_array = reshape(reordered_fit_array, (n_survivors_per_round, t_size))
        # now get 1 winner from each row
        winning_indices = transpose(getindex.(selector(reordered_fit_array, dims=2), 2)) # is the reshape necessary?
        for j ∈ 1:length(winning_indices)
            winning_indices[j] += (j-1)*t_size
        end
        println(winning_indices)
        # store the chosen ones
        survivors = append!(survivors, pop[random_order[winning_indices]])
    end
    survivors 
end
*/


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
        for(int j = 0; j < n_selected_per_round; j++) {
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