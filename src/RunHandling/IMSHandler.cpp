/*
 


 */

/* 
 * File:   IMSHandler.cpp
 * Author: virgolin
 * 
 * Created on September 6, 2018, 1:38 PM
 */

#include "GPGOMEA/RunHandling/IMSHandler.h"

using namespace std;

void IMSHandler::Start() {

    string stats_file = "stats_generations.txt";
    string heading = "gen\ttime\tevals\tbest_fit\tbest_size\tpop_size";
    if (!st->config->running_from_python)
        Logger::GetInstance()->Log(heading, stats_file);

    macro_generation = 0;

    if (!st->config->use_IMS)
        max_num_runs = 1;

    size_t maximum_arity_functions = 0;
    for (Operator * op : st->config->functions) {
        if (op->arity > maximum_arity_functions) {
            maximum_arity_functions = op->arity;
        }
    }

    // begin timer
    st->timer.tic();

    size_t current_pop_size = st->config->population_size;
    size_t biggest_pop_size_reached = 0;

    while (true) {

        // Check if termination criteria is met
        if ((st->fitness->evaluations > 0 && st->fitness->evaluations >= st->config->max_evaluations) ||
                (st->config->max_generations > 0 && macro_generation >= st->config->max_generations) ||
                (st->config->max_time > 0 && st->timer.toc() > st->config->max_time)) {
            break;
        } else if (terminated_runs[max_num_runs - 1]) {
            break;
        }

        for (int i = min_run_idx; i <= min(max_run_idx + 1, (int) max_num_runs - 1); i++) {

            // determine previous active run
            int prev_run_idx = -1;
            for (int j = i - 1; j >= min_run_idx; j--) {
                if (!terminated_runs[j]) {
                    prev_run_idx = j;
                    break;
                }
            }

            // check if run should start
            if (!terminated_runs[i] && // must not be terminated, AND
                    (i == min_run_idx // it is the smallest run, OR
                    || (subgenerations_performed[prev_run_idx] == pow((int) st->config->num_sugen_IMS, i - prev_run_idx)) // previous active run did enough subgenerations
                    )) {

                if (i != min_run_idx) { // reset counter for previous run
                    subgenerations_performed[prev_run_idx] = 0;
                }

                // check if run needs to be initialized
                if (!initialized_runs[i]) {
                    if (max_num_runs > 1)
                        cout << " ! IMS: ";
                    cout << " Initialized run " << i << " with population size " << current_pop_size << " and initial tree height " << st->config->initial_maximum_tree_height << endl;
                    st->config->population_size = current_pop_size;
                    runs[i] = new EvolutionRun(*st);

                    runs[i]->Initialize();
                    initialized_runs[i] = true;
                    biggest_pop_size_reached = current_pop_size;
                    current_pop_size *= 2;

                    max_run_idx++;
                }

                // Perform generation
                runs[i]->DoGeneration();

                // check if new fitness is better
                if (runs[i]->elitist_fit < elitist_per_run_fit[i]) {
                    elitist_per_run_fit[i] = runs[i]->elitist_fit;
                    if (elitist_per_run[i]) {
                        elitist_per_run[i]->ClearSubtree();
                    }
                    elitist_per_run[i] = runs[i]->elitist->CloneSubtree();
                }
                if (runs[i]->elitist_fit < elitist_fit) {
                    elitist_fit = runs[i]->elitist_fit;
                    elitist_size = runs[i]->elitist->GetSubtreeNodes(true).size();

                    // reset the count on the number of times this run performed poorly
                    number_of_times_run_was_worse_than_later_run[i] = 0;
                } else {
                    // check if this run is performing poorly:
                    // determine if subsequent run exists with better fitness
                    for (int j = i + 1; j < max_run_idx; j++) {
                        if (initialized_runs[j] && !terminated_runs[j]) {
                            if (elitist_per_run_fit[j] < elitist_per_run_fit[i]) {
                                number_of_times_run_was_worse_than_later_run[i] = number_of_times_run_was_worse_than_later_run[i] + 1;
                                break;
                            }
                        }
                    }
                }

                // increment subgeneration performed by this run
                subgenerations_performed[i] = subgenerations_performed[i] + 1;
            }

            // check if run should terminate
            if (max_num_runs > 1 && initialized_runs[i] && !terminated_runs[i]) {

                bool should_terminate = false;
                // check if it should terminate
                if (initialized_runs[i] && st->config->gomea && ((GOMEAGenerationHandler*) runs[i]->generation_handler)->gomea_converged) {
                    should_terminate = true;
                }
                if (initialized_runs[i] && number_of_times_run_was_worse_than_later_run[i] >= st->config->early_stopping_IMS) {
                    should_terminate = true;
                }

                if (should_terminate) {
                    terminated_runs[i] = true;
                    delete runs[i];
                    runs[i] = NULL;

                    if (i == min_run_idx) {
                        for (size_t j = min_run_idx + 1; j <= max_run_idx + 1; j++) {
                            if (!terminated_runs[j]) {
                                min_run_idx = j;
                                break;
                            }
                        }
                        if (max_run_idx + 1 < min_run_idx) {
                            max_run_idx++;
                        }
                    }
                    cout << " ! IMS: terminated run " << i << endl;
                }
            }
        }

        macro_generation++;

        string generation_stats = to_string(macro_generation) + "\t" + to_string(st->timer.toc()) + "\t" + to_string(st->fitness->evaluations) + "\t" + to_string(elitist_fit) + "\t" + to_string(elitist_size) + "\t" + to_string(biggest_pop_size_reached);

        if (!st->config->running_from_python)
            Logger::GetInstance()->Log(generation_stats, stats_file);

        cout << " > generation " << macro_generation << " - best fit: " << elitist_fit << endl;

    }

    Terminate();

}

Node * IMSHandler::GetFinalElitist() {
    Node * final_elitist;
    size_t final_elitist_idx = 0;
    if (st->fitness->ValidationY.empty()) {
        for (size_t i = 0; i < elitist_per_run_fit.size(); i++) {
            if (elitist_per_run_fit[i] == elitist_fit) {
                final_elitist_idx = i;
                break;
            }
        }
    } else {
        // compute the best solution according to the validation set
        double_t best_validation_fitness = arma::datum::inf;
        for (size_t i = 0; i < elitist_per_run.size(); i++) {
            if (!elitist_per_run[i])
                continue;
            double_t val_fit = st->fitness->GetValidationFit(elitist_per_run[i]);
            if (val_fit < best_validation_fitness) {
                final_elitist_idx = i;
                best_validation_fitness = val_fit;
            }
        }
    }
    final_elitist = elitist_per_run[final_elitist_idx];
    return final_elitist;
}

std::vector<Node*> IMSHandler::GetAllActivePopulations(bool copy_solutions) {
    vector<Node*> result;
    result.reserve(10000);
    for(int i = 0 ; i < runs.size(); i++) {
        if (runs[i]){
            for(Node * n : runs[i]->population) {
                if (copy_solutions)
                    result.push_back(n->CloneSubtree());
                else
                    result.push_back(n);
            }
        }
    }
    return result;
}

void IMSHandler::Terminate() {
    // Terminate
    cout << " -=-=-=-=-=-= TERMINATED =-=-=-=-=-=- " << endl;

    string msg = "";
    string out = "";

    // find best solution
    Node * final_elitist = GetFinalElitist();

    // best solution found
    out = "Best solution found:\t" + final_elitist->GetSubtreeHumanExpression() + "\n";
    cout << out;
    msg += out;

    if (st->config->linear_scaling) {
        pair<double_t, double_t> ab = Utils::ComputeLinearScalingTerms(final_elitist->GetOutput(st->fitness->TrainX, st->config->caching), st->fitness->TrainY, &st->fitness->trainY_mean, &st->fitness->var_comp_trainY);
        out = "Linear scaling coefficients:\ta=" + to_string(ab.first) + "\tb=" + to_string(ab.second) + "\n";
        cout << out;
        msg += out;
    }

    out = "Number of nodes:\t" + to_string(final_elitist->GetSubtreeNodes(true).size()) + "\n";
    cout << out;
    msg += out;

    // Training fitness
    double_t training_fit = st->fitness->ComputeFitness(final_elitist, false);
    out = "Train fit:\t" + to_string(training_fit) + "\n";
    cout << out;
    msg += out;

    // Validation fitness
    if (!st->fitness->ValidationY.empty()) {
        double_t validation_fit = st->fitness->GetValidationFit(final_elitist);
        out = "Validation fit:\t" + to_string(validation_fit) + "\n";
        cout << out;
        msg += out;
    }

    if (!st->fitness->TestY.empty()) {
        double_t test_fit = st->fitness->GetTestFit(final_elitist);
        out = "Test fit:\t" + to_string(test_fit) + "\n";
        cout << out;
        msg += out;
    }

    // Running time
    out = "Running time:\t" + to_string(std::round(st->timer.toc()*100) / 100) + "\n";
    cout << out;
    msg += out;

    // Evaluations
    out = "Evaluations:\t" + to_string(st->fitness->evaluations) + "\n";
    cout << out;
    msg += out;

    if (!st->config->running_from_python)
        Logger::GetInstance()->Log(msg, "result.txt");
}
