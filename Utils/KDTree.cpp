/*
 


 */

#include "KDTree.h"

using namespace kdt;
using namespace std;
using namespace arma;

void KDTree::build(const std::vector<arma::vec> & points) {
    clear();

    points_ = points;
    _dim = points[0].size();

    std::vector<int> indices(points.size());
    std::iota(std::begin(indices), std::end(indices), 0);

    root_ = buildRecursive(indices.data(), (int) points.size(), 0);
}

void KDTree::addPoint(arma::vec point) {
    points_.push_back(point);
}

std::vector<int> KDTree::radiusSearch(const arma::vec& query, double_t radius) {
    std::vector<int> indices;
    radiusSearchRecursive(query, root_, indices, radius);
    return indices;
}

void KDTree::radiusSearchRecursive(const arma::vec& query, const Node* node, std::vector<int>& indices, double_t radius) {
    if (node == NULL)
        return;

    const arma::vec& train = points_[node->idx];

    double_t dist = Utils::ComputeDistance(query, train);
    if (dist < radius)
        indices.push_back(node->idx);

    const int axis = node->axis;
    const int dir = query[axis] < train[axis] ? 0 : 1;
    radiusSearchRecursive(query, node->next[dir], indices, radius);

    const double_t diff = fabs(query[axis] - train[axis]);
    if (diff < radius)
        radiusSearchRecursive(query, node->next[!dir], indices, radius);
}

void KDTree::build() {

    if (root_)
        clear();

    if (points_.empty()) {
        return;
    }
    _dim = points_[0].size();
    std::vector<int> indices(points_.size());
    std::iota(std::begin(indices), std::end(indices), 0);

    root_ = buildRecursive(indices.data(), (int) points_.size(), 0);
}

KDTree::Node* KDTree::buildRecursive(int* indices, int npoints, int depth) {

    if (npoints <= 0)
        return NULL;

    const int axis = depth % _dim;
    const int mid = (npoints - 1) / 2;

    std::nth_element(indices, indices + mid, indices + npoints, [&](int lhs, int rhs) {
        return points_[lhs][axis] < points_[rhs][axis];
    });

    Node * node = new Node();
    node->idx = indices[mid];
    node->axis = axis;

    node->next[0] = buildRecursive(indices, mid, depth + 1);
    node->next[1] = buildRecursive(indices + mid + 1, npoints - mid - 1, depth + 1);

    return node;
}

void KDTree::clear() {
    clearRecursive(root_);
    root_ = NULL;
    points_.clear();
}

bool KDTree::validate() {
    try {
        validateRecursive(root_, 0);
    } catch (const Exception&) {
        return false;
    }

    return true;
}

void KDTree::validateRecursive(const Node* node, int depth) {
    if (node == NULL)
        return;

    const int axis = node->axis;
    const Node* node0 = node->next[0];
    const Node* node1 = node->next[1];

    if (node0 && node1) {
        if (points_[node->idx][axis] < points_[node0->idx][axis])
            throw Exception();

        if (points_[node->idx][axis] > points_[node1->idx][axis])
            throw Exception();
    }

    if (node0)
        validateRecursive(node0, depth + 1);

    if (node1)
        validateRecursive(node1, depth + 1);
}

arma::vec* KDTree::getPoint(int index) {
    return & points_[index];
}

KDTree::KDTree(const std::vector<arma::vec>& points) : root_(NULL) {
    build(points);
}

KDTree::KDTree() : root_(NULL) {
}

KDTree::~KDTree() {
    clear();
}

std::vector<arma::vec*> KDTree::getPoints(const std::vector<int>& indices) {
    std::vector< arma::vec * > to_return;
    for (int i = 0; i < indices.size(); i++) {
        to_return.push_back(& points_[indices[i]]);
    }
    return to_return;
}

size_t KDTree::getPointsNumber() {
    return points_.size();
}

void KDTree::clearRecursive(Node* node) {
    if (node == NULL)
        return;

    if (node->next[0])
        clearRecursive(node->next[0]);

    if (node->next[1])
        clearRecursive(node->next[1]);

    delete node;
}

void KDTree::deletePoint(arma::vec point) {
    for (auto it = points_.begin(); it != points_.end(); it++) {
        arma::vec & p = (*it);
        if (Utils::HashArmaVec(p) == Utils::HashArmaVec(point)) {
            points_.erase(it);
            break;
        }
    }
}

std::vector<int> KDTree::knnSearch(const arma::vec& query, int k) {
    KnnQueue queue(k);
    knnSearchRecursive(query, root_, queue, k);

    std::vector<int> indices(queue.size());
    for (size_t i = 0; i < queue.size(); i++)
        indices[i] = queue[i].second;

    return indices;
}

void KDTree::knnSearchRecursive(const arma::vec& query, const Node* node, KnnQueue& queue, int k) {
    if (node == NULL)
        return;

    const arma::vec& train = points_[node->idx];

    double_t dist = Utils::ComputeDistance(query, train);
    queue.push(std::make_pair(dist, node->idx));

    const int axis = node->axis;
    const int dir = query[axis] < train[axis] ? 0 : 1;
    knnSearchRecursive(query, node->next[dir], queue, k);

    const double_t diff = fabs(query[axis] - train[axis]);
    if ((int) queue.size() < k || diff < queue.back().first)
        knnSearchRecursive(query, node->next[!dir], queue, k);
}

int KDTree::nnSearch(const arma::vec& query, double_t* minDist, bool no_perfect_match) {
    int guess;
    double_t _minDist = arma::datum::inf;

    nnSearchRecursive(query, root_, &guess, &_minDist, no_perfect_match);

    if (minDist)
        *minDist = _minDist;

    return guess;
}

int KDTree::nnSearch(const std::vector<arma::vec>& query, double_t* minDist, bool no_perfect_match) {
    int guess;
    double_t _minDist = arma::datum::inf;

    nnSearchRecursive(query, root_, &guess, &_minDist, no_perfect_match);

    if (minDist)
        *minDist = _minDist;

    return guess;
}

void KDTree::nnSearchRecursive(const arma::vec & query, const Node* node, int* guess, double_t * minDist, bool no_perfect_match) {

    if (!node)
        return;

    const arma::vec& train = points_[node->idx];

    double_t dist = Utils::ComputeDistance(query, train);
    if (!no_perfect_match || (no_perfect_match && dist != 0)) {
        if (dist < *minDist) {
            *minDist = dist;
            *guess = node->idx;
        }
    } else {
        dist = *minDist;
    }

    const int axis = node->axis;
    const int dir = query[axis] < train[axis] || std::isnan(query[axis]) ? 0 : 1;

    nnSearchRecursive(query, node->next[dir], guess, minDist, no_perfect_match);

    const double_t diff = fabs(query[axis] - train[axis]);
    if (diff < *minDist || std::isnan(query[axis]))
        nnSearchRecursive(query, node->next[!dir], guess, minDist, no_perfect_match);
}

void KDTree::nnSearchRecursive(const std::vector<arma::vec>& query, const Node * node, int* guess, double_t * minDist, bool no_perfect_match) {

    if (!node)
        return;

    const arma::vec& train = points_[node->idx];

    arma::vec generated_query(query.size());

    double_t dist =  Utils::ComputeDistanceWithDontCares(query, train, &generated_query);

    if (!no_perfect_match || (no_perfect_match && dist != 0)) {
        if (dist < *minDist) {
            *minDist = dist;
            *guess = node->idx;
        }
    } else {
        dist = *minDist;
    }

    const int axis = node->axis;
    int dir = 1;
    if (generated_query[axis] < train[axis])
        dir = 0;

    nnSearchRecursive(query, node->next[dir], guess, minDist, no_perfect_match);

    const double_t diff = fabs(generated_query[axis] - train[axis]);
    if (diff < *minDist)
        nnSearchRecursive(query, node->next[!dir], guess, minDist, no_perfect_match);
}
