// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <armadillo>
#include <iostream>
#include <sstream>
#include <fstream>

#include "GPGOMEA/Evolution/EvolutionState.h"
#include "GPGOMEA/Evolution/EvolutionRun.h"
#include "GPGOMEA/RunHandling/IMSHandler.h"
#include "GPGOMEA/Genotype/Node.h"
#include "GPGOMEA/Utils/Utils.h"
#include "GPGOMEA/Operators/OpVariable.h"
#include "GPGOMEA/Operators/Regression/OpRegrConstant.h"

using namespace Rcpp;

static inline arma::mat to_arma_mat(const NumericMatrix& X) {
    // arma::mat: memory not owned (copy_aux_mem = false)
    return arma::mat(const_cast<double*>(X.begin()), X.nrow(), X.ncol(), false, false);
}

static inline arma::vec to_arma_vec(const NumericVector& y) {
    return arma::vec(const_cast<double*>(y.begin()), y.size(), false, false);
}

struct Rgpgomea {
    Node * solution = nullptr;
    EvolutionState * st = nullptr;
    IMSHandler * imsh = nullptr;
    std::string hyperparams_string = "";
    std::string progress_log = "";
    std::streambuf * default_output_stream = std::cout.rdbuf();
    std::ofstream * output_file_stream = nullptr;
    std::stringstream * silent_output_stream = nullptr;
    std::vector<Node*> final_population;
    size_t evaluations = 0;

    explicit Rgpgomea(const std::string& hyperparams_string_) {
        hyperparams_string = hyperparams_string_;

        bool silent = false;
        silent_output_stream = new std::stringstream();
        output_file_stream = new std::ofstream();
        bool log_to_file = false;
        std::string logfile;

        auto opts = Utils::SplitStringByChar(hyperparams_string, ' ');
        std::vector<std::string> argv_v = {"dummy"};
        argv_v.reserve(100);
        for (size_t i = 0; i < opts.size(); i++) {
            if (opts[i] == "--silent") {
                silent = true;
                continue;
            }
            if (opts[i] == "--logtofile") {
                log_to_file = true;
                logfile = opts[++i];
                continue;
            }
            argv_v.push_back(opts[i]);
        }
        size_t argc = argv_v.size();
        std::vector<char*> argv(argc, nullptr);
        for (size_t i = 0; i < argc; i++){
            argv[i] = const_cast<char*>(argv_v[i].c_str());
        }
        if (silent) {
            std::cout.rdbuf(silent_output_stream->rdbuf());
        } else {
            std::cout.rdbuf(default_output_stream);
        }
        if (log_to_file) {
            output_file_stream = new std::ofstream(logfile.c_str());
            std::cout.rdbuf(output_file_stream->rdbuf());
        }

        st = new EvolutionState();
        st->config->running_from_python = true; // python hack
        st->SetOptions((int)argc, argv.data());
    }

    ~Rgpgomea() {
        reset();
    }

    void reset() {
        if (solution) {
            solution->ClearSubtree();
            solution = nullptr;
        }
        if (imsh) {
            delete imsh;
            imsh = nullptr;
        }
        if (st) {
            delete st;
            st = nullptr;
        }
        if (silent_output_stream) {
            delete silent_output_stream;
            silent_output_stream = nullptr;
        }
        if (output_file_stream) {
            delete output_file_stream;
            output_file_stream = nullptr;
        }
        for (Node * n : final_population) {
            if (n) n->ClearSubtree();
        }
        final_population.clear();
        std::cout.rdbuf(default_output_stream);
        evaluations = 0;
        progress_log.clear();
    }

    void run(const NumericMatrix& Xr, const NumericVector& yr) {
        arma::mat X = to_arma_mat(Xr);
        arma::vec Y = to_arma_vec(yr);

        if (X.n_rows != Y.n_rows) {
            stop("run: X and y must have compatible number of rows");
        }

        arma::mat Ycol(Y.n_rows, 1);
        Ycol.col(0) = Y;
        arma::mat TR = arma::join_horiz(X, Ycol);

        if (st->config->validation_perc > 0) {
            arma::mat V;
            size_t nrows_validation = (size_t)std::floor(TR.n_rows * st->config->validation_perc);
            while (V.n_rows < nrows_validation && TR.n_rows > 0) {
                size_t which_row = std::min(
                    (size_t)std::floor(arma::randu() * TR.n_rows),
                    (size_t)(TR.n_rows - 1)
                    );
                arma::mat x = TR.row(which_row);
                V = join_vert(V, x);
                TR.shed_row(which_row);
            }
            st->fitness->SetFitnessCases(V, FitnessCasesType::FitnessCasesVALIDATION);
        }
        st->fitness->SetFitnessCases(TR, FitnessCasesType::FitnessCasesTRAIN);
        st->fitness->SetFitnessCases(TR, FitnessCasesType::FitnessCasesTEST);

        // Terminal variables (one per column)
        for (size_t i = 0; i < (size_t)X.n_cols; i++) {
            st->config->terminals.push_back(new OpVariable(i));
        }

        // ERC with SR
        if (st->config->use_ERC) {
            double_t biggest_val = arma::max(arma::max(arma::abs(st->fitness->TrainX)));
            double_t min_erc = -5 * biggest_val;
            double_t max_erc = 5 * biggest_val;
            st->config->terminals.push_back(new OpRegrConstant(min_erc, max_erc));
        }

        imsh = new IMSHandler(st);
        st->fitness->evaluations = 0;
        imsh->Start();

        solution = imsh->GetFinalElitist()->CloneSubtree();
        evaluations = st->fitness->evaluations;
        final_population = imsh->GetAllActivePopulations(true);
        progress_log = imsh->progress_log;

        delete imsh;
        imsh = nullptr;
    }

    NumericVector predict(const NumericMatrix& Xr) {
        if (!solution){
            stop("predict: called before run");
        }
        arma::mat X = to_arma_mat(Xr);
        arma::vec out = solution->GetOutput(X, false);

        std::pair<double_t,double_t> ab(0.0, 1.0);
        if (st->config->linear_scaling) {
            arma::vec trainout = solution->GetOutput(st->fitness->TrainX, st->config->caching);
            ab = Utils::ComputeLinearScalingTerms(trainout, st->fitness->TrainY, &st->fitness->trainY_mean, &st->fitness->var_comp_trainY);
        }
        out = ab.first + out * ab.second;

        return NumericVector(out.begin(), out.end());
    }

    double score(const NumericMatrix& Xr, const NumericVector& yr) {
        if (!solution) {
            stop("score: called before run");
        }

        arma::mat X = to_arma_mat(Xr);
        arma::vec Y = to_arma_vec(yr);
        if (X.n_rows != Y.n_rows) {
            stop("score: X and y must have compatible number of rows");
        }
        arma::mat Ycol(Y.n_rows, 1);
        Ycol.col(0) = Y;

        arma::mat TE = arma::join_horiz(X, Ycol);
        st->fitness->SetFitnessCases(TE, FitnessCasesType::FitnessCasesTEST);
        double_t neg_err = -1.0 * st->fitness->GetTestFit(solution);
        return neg_err;
    }

    std::string get_model() {
        if (!solution) {
            stop("get_model: called before run");
        }
        std::string human_expression = solution->GetSubtreeHumanExpression();
        if (st->config->linear_scaling) {
            arma::vec trainout = solution->GetOutput(st->fitness->TrainX, st->config->caching);
            auto ab = Utils::ComputeLinearScalingTerms(trainout, st->fitness->TrainY, &st->fitness->trainY_mean, &st->fitness->var_comp_trainY);
            human_expression = std::to_string(ab.first) + "+" + std::to_string(ab.second) + "*(" + human_expression + ")";
        }
        return human_expression;
    }

    std::string get_progress_log() {
        return progress_log;
    }

    size_t get_n_nodes() {
        if (!solution) {
            stop("get_n_nodes: called before run");
        }
        return solution->GetSubtreeNodes(true).size();
    }

    size_t get_evaluations() {
        return evaluations;
    }

    List get_final_population(const NumericMatrix& Xr) {
        if (!solution || final_population.empty()) {
            stop("get_final_population: called before run");
        }
        arma::mat X = to_arma_mat(Xr);

        List out;
        for (Node * n : final_population) {
            std::pair<double_t,double_t> ab(0.0, 1.0);
            std::string prefix_expression = n->GetSubtreeExpression();
            std::string human_expression = n->GetSubtreeHumanExpression();
            if (st->config->linear_scaling) {
                arma::vec trainout = n->GetOutput(st->fitness->TrainX, st->config->caching);
                ab = Utils::ComputeLinearScalingTerms(trainout, st->fitness->TrainY, &st->fitness->trainY_mean, &st->fitness->var_comp_trainY);
                prefix_expression = "+" + std::to_string(ab.first) + "*" + std::to_string(ab.second) + prefix_expression;
                human_expression = std::to_string(ab.first) + "+" + std::to_string(ab.second) + "*(" + human_expression + ")";
            }
            arma::vec pred = n->GetOutput(X, false);
            pred = ab.first + pred * ab.second;

            if (n->cached_objectives.n_elem > 1) {
                double second_obj = n->cached_objectives[1];
                out.push_back(List::create(
                    _["prediction"] = NumericVector(pred.begin(), pred.end()),
                    _["fitness"] = n->cached_fitness,
                    _["second_objective"] = second_obj,
                    _["human_expression"] = human_expression,
                    _["prefix_expression"] = prefix_expression
                ));
            } else {
                out.push_back(List::create(
                    _["prediction"] = NumericVector(pred.begin(), pred.end()),
                    _["fitness"] = n->cached_fitness,
                    _["human_expression"] = human_expression,
                    _["prefix_expression"] = prefix_expression
                ));
            }
        }
        return out;
    }
};

// Descructor XPtr
static void rgpgomea_finalizer(SEXP p) {
    if (!R_ExternalPtrAddr(p)) return;
    auto ptr = reinterpret_cast<Rgpgomea*>(R_ExternalPtrAddr(p));
    delete ptr;
    R_ClearExternalPtr(p);
}

// [[Rcpp::export]]
SEXP gpg_new(std::string hyperparams) {
    XPtr<Rgpgomea> ptr(new Rgpgomea(hyperparams), true);
    R_RegisterCFinalizerEx(ptr, rgpgomea_finalizer, TRUE);
    return ptr;
}

// [[Rcpp::export]]
void gpg_run(SEXP handle, NumericMatrix X, NumericVector y) {
    XPtr<Rgpgomea> ptr(handle);
    ptr->run(X, y);
}

// [[Rcpp::export]]
NumericVector gpg_predict(SEXP handle, NumericMatrix X) {
    XPtr<Rgpgomea> ptr(handle);
    return ptr->predict(X);
}

// [[Rcpp::export]]
double gpg_score(SEXP handle, NumericMatrix X, NumericVector y) {
    XPtr<Rgpgomea> ptr(handle);
    return ptr->score(X, y);
}

// [[Rcpp::export]]
std::string gpg_get_model(SEXP handle) {
    XPtr<Rgpgomea> ptr(handle);
    return ptr->get_model();
}

// [[Rcpp::export]]
std::string gpg_get_progress_log(SEXP handle) {
    XPtr<Rgpgomea> ptr(handle);
    return ptr->get_progress_log();
}

// [[Rcpp::export]]
double gpg_get_n_nodes(SEXP handle) {
    XPtr<Rgpgomea> ptr(handle);
    return static_cast<double>(ptr->get_n_nodes());
}

// [[Rcpp::export]]
double gpg_get_evaluations(SEXP handle) {
    XPtr<Rgpgomea> ptr(handle);
    return static_cast<double>(ptr->get_evaluations());
}

// [[Rcpp::export]]
List gpg_get_final_population(SEXP handle, NumericMatrix X) {
    XPtr<Rgpgomea> ptr(handle);
    return ptr->get_final_population(X);
}
