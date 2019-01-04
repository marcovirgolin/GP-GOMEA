/*
 


 */

/* 
 * File:   Utils.cpp
 * Author: virgolin
 * 
 * Created on June 28, 2018, 10:41 AM
 */

#include "Utils.h"

using namespace std;
using namespace arma;

std::string Utils::ReplaceCharInString(const std::string original, char to_replace, char replacing) {
    std::string new_one(original);
    for (int i = 0; i < new_one.length(); ++i) {
        if (new_one[i] == to_replace)
            new_one[i] = replacing;
    }
    return new_one;
}

std::string Utils::ToLowerCase(const std::string original) {
    string lc = original;
    std::transform(lc.begin(), lc.end(), lc.begin(), ::tolower);
    return lc;
}

std::vector<std::string> Utils::SplitStringByChar(const std::string original, char splitc) {
    std::vector<std::string> result;
    std::string changed_string = original;
    if (splitc != ' ') {
        changed_string = Utils::ReplaceCharInString(original, splitc, ' ');
    }
    std::istringstream iss(changed_string);
    result = {std::istream_iterator<std::string>
        {iss}, std::istream_iterator<std::string>
        {}};
    return result;
}

std::pair<double_t, double_t> Utils::ComputeMeanStdEfficiently(const arma::vec& x) {
    double_t m = mean(x);
    arma::vec xnm = x - m;
    double_t sd = std::sqrt(arma::sum(arma::square(xnm)) / x.n_elem);

    return make_pair(m, sd);
}

std::pair<double_t, double_t> Utils::ComputeMeanStdEfficiently(const std::vector<double_t> & x) {
    double_t m = 0.0;
    for (size_t i = 0; i < x.size(); i++) {
        m += x[i];
    }
    m /= x.size();

    double_t sd = 0.0;

    for (size_t i = 0; i < x.size(); i++) {
        sd += (x[i] - m)*(x[i] - m);
    }
    sd /= x.size();
    sd = std::sqrt(sd);

    return make_pair(m, sd);
}

arma::vec Utils::Normalize(const arma::vec& x) {
    double_t m = mean(x);
    arma::vec xnm = x - m;
    double_t sd = std::sqrt(arma::sum(arma::square(xnm)) / x.n_elem);
    return xnm / sd;
}

size_t Utils::HashArmaVec(const arma::vec& x) {

    vector<double_t> y;
    y.reserve(x.n_elem);
    for (size_t i = 0; i < x.n_elem; i++) {
        y.push_back(x[i]);
    }

    return boost::hash_value(y);

}

bool Utils::IsNumber(const std::string s) {
    string cp = s;
    if (cp[0] == '-')
        cp.erase(0, 1);

    return !cp.empty() && std::find_if(cp.begin(),
            cp.end(), [](char c) {
                return !(std::isdigit(c) || c == '.'); }) == cp.end();
}

std::pair<double_t, double_t> Utils::ComputeLinearScalingTerms(const arma::vec& P, const arma::vec& Y, double_t* precomputed_mean_Y, arma::vec* precomputed_var_terms_Y, double_t * precomputed_mean_P, double_t * precomputed_var_terms_P, double_t * precomputed_denom_P) {

    double_t mean_Y;
    vec var_terms_Y;

    if (precomputed_mean_Y) {
        mean_Y = (*precomputed_mean_Y);
    } else {
        mean_Y = mean(Y);
    }

    if (precomputed_var_terms_Y) {
        var_terms_Y = (*precomputed_var_terms_Y);
    } else {
        var_terms_Y = Y - mean_Y;
    }

    double_t mean_P;
    if (precomputed_mean_P)
        mean_P = *precomputed_mean_P;
    else
        mean_P = arma::mean(P);
    
    arma::vec var_terms_P;
    if (precomputed_var_terms_P)
        var_terms_P = *precomputed_var_terms_P;
    else
        var_terms_P = (P - mean_P);

    double_t denom_P;
    if (precomputed_denom_P)
        denom_P = *precomputed_denom_P;
    else
        denom_P = arma::sum(arma::square(var_terms_P));

    double_t a, b;
    if (denom_P != 0) {
        b = arma::sum(var_terms_Y % var_terms_P) / denom_P;
        a = mean_Y - b * mean_P;
    } else {
        b = 0;
        a = mean_Y;
    }

    return make_pair(a, b);

}

double_t Utils::ComputeDistanceWithDontCares(const std::vector<arma::vec> & x, const arma::vec& y, arma::vec * new_x) {

    double_t dist = 0;

    for (int i = 0; i < x.size(); i++) {

        double_t min_among_vals_dist = arma::datum::inf;
        arma::vec vals = x[i];

        if (new_x)
            (*new_x)[i] = arma::datum::inf;

        for (int j = 0; j < vals.n_elem; j++) {

            if (std::isnan(vals[j])) {
                min_among_vals_dist = 0;
                if (new_x)
                    (*new_x)[i] = y[i];
                break;
            }

            double_t cur_dist = (vals[j] - y[i])*(vals[j] - y[i]);

            if (cur_dist <= min_among_vals_dist) { // still do this for don't cares in order to build up the query to search for neighbours in KDtree
                min_among_vals_dist = cur_dist;
                if (new_x)
                    (*new_x)[i] = vals[j];
            }
        }

        dist += min_among_vals_dist;
    }
    //dist = sqrt(dist);

    return dist;
}

double_t Utils::ComputeDistance(const arma::vec& x, const arma::vec& y, bool linear_scaling, double_t * precomputed_mean_Y, arma::vec * precomputed_var_terms_Y, double_t * precomputed_mean_P, double_t * precomputed_var_terms_P, double_t * precomputed_denom_P) {
    double_t dist;
    if (linear_scaling) {
        pair<double_t, double_t> ab = ComputeLinearScalingTerms(y, x, precomputed_mean_Y, precomputed_var_terms_Y, precomputed_mean_P, precomputed_var_terms_P, precomputed_denom_P);
        dist = arma::sum(arma::square(x - (ab.first + ab.second * y)));
    } else {
        dist = arma::sum(arma::square(x - y));
    }
    //dist = sqrt(dist);
    return dist;
}
