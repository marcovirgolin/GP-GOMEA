#include "GPGOMEA/Evolution/NSGA2GenerationHandler.h"

using namespace std;
using namespace arma;

void NSGA2GenerationHandler::PerformGeneration(std::vector<Node*> & population) {

	// Keep track of normal pop_size
	size_t pop_size = population.size();

	// Parent selection based on MO tournaments
	vector<Node*> selected_parents; selected_parents.reserve(population.size());
	while(selected_parents.size() < pop_size) {
		selected_parents.push_back(TournamentSelection::GetMOTournamentSelectionWinner(population, conf->tournament_selection_size));
	}

    // Variation
    vector<Node*> offspring = MakeOffspring(population, selected_parents);

    // Update fitness
    fitness->GetPopulationFitness(offspring, true, conf->caching);

    // P+O
    population.insert(population.end(), offspring.begin(), offspring.end());

    // Assign ranks
    vector<vector<Node*>> fronts = FastNonDominatedSorting(population);

	// Pick survivors
    vector<Node*> selection; selection.reserve(pop_size);
    size_t current_front_idx = 0;
    while ( current_front_idx < fronts.size() && 
    	fronts[current_front_idx].size() + selection.size() < pop_size ) {
    	
    	ComputeCrowndingDistance(fronts[current_front_idx]);
    	selection.insert(selection.end(), fronts[current_front_idx].begin(), fronts[current_front_idx].end());
    	current_front_idx++;
    }

    // insert remaining solutions
    if (selection.size() < pop_size) {
    	ComputeCrowndingDistance(fronts[current_front_idx]);
    	sort( fronts[current_front_idx].begin(), fronts[current_front_idx].end(), [](const Node * lhs, const Node * rhs) 
	    	{
	    		return lhs->crowding_distance > rhs->crowding_distance;
	    	});

    	while(selection.size() < pop_size) {
    		selection.push_back(fronts[current_front_idx][0]);
    		fronts[current_front_idx].erase(fronts[current_front_idx].begin()); // remove first element
    	}
    }

    // cleanup leftovers
    for(size_t i=current_front_idx; i < fronts.size(); i++) {
    	for (Node * n : fronts[i]) {
    		n->ClearSubtree();
    	}
    }

    // Update population
    population = selection;
}


std::vector<std::vector<Node*>> NSGA2GenerationHandler::FastNonDominatedSorting(std::vector<Node*> & population){
	size_t rank_counter = 0;
	vector<vector<Node*>> nondominated_fronts; nondominated_fronts.reserve(10);
	unordered_map<Node*, std::vector<Node*>> dominated_solutions;
	unordered_map<Node*, int> domination_counts;
	vector<Node*> current_front; current_front.reserve(population.size() / 2);


	for (size_t i = 0; i < population.size(); i++) {
		Node * p = population[i];

		dominated_solutions[p].reserve(population.size() / 2);
		domination_counts[p] = 0;

		for(size_t j = 0; j < population.size(); j++) {
			if (i == j)
				continue;
			Node * q = population[j];

			if(p->Dominates(q))
				dominated_solutions[p].push_back(q);
			else if (q->Dominates(p))
				domination_counts[p]++;
		}

		if (domination_counts[p] == 0) {
			p->rank = rank_counter;
			current_front.push_back(p);
		}
	}

	while(current_front.size() > 0) {
		vector<Node*> next_front; next_front.reserve(population.size() / 2);
		for(Node * p : current_front) {
			for(Node * q : dominated_solutions[p]) {
				domination_counts[q] -= 1;
				if (domination_counts[q] == 0) {
					q->rank = rank_counter + 1;
					next_front.push_back(q);
				}
			}
		}
		nondominated_fronts.push_back(current_front);
		rank_counter++;
		current_front = next_front;
	}

	return nondominated_fronts;
}


void NSGA2GenerationHandler::ComputeCrowndingDistance(std::vector<Node *> & front){
	size_t number_of_objs = front[0]->cached_objectives.size();
	size_t front_size = front.size();

	for (Node * p : front) {
		p->crowding_distance = 0;
	}

	for(size_t i = 0; i < number_of_objs; i++) {
		std::sort(front.begin(), front.end(), [i](const Node * lhs, const Node * rhs)
			{
			    return lhs->cached_objectives[i] < rhs->cached_objectives[i];
			});

		front[0]->crowding_distance = datum::inf;
		front[front.size() - 1]->crowding_distance = datum::inf;

		double_t min_obj = front[0]->cached_objectives[i];
		double_t max_obj = front[front.size() - 1]->cached_objectives[i];

		if (min_obj == max_obj)
			continue;

		for(size_t j = 1; j < front.size() - 1; j++) {
			if (isinf(front[j]->crowding_distance))
				continue;

			double_t prev_obj, next_obj;
			prev_obj = front[j-1]->cached_objectives[i];
			next_obj = front[j+1]->cached_objectives[i];

			front[j]->crowding_distance += (next_obj - prev_obj)/(max_obj - min_obj);
		}
	}
}