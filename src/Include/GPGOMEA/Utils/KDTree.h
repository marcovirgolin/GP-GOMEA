/* 
 * File:   KDTree.h
 * Author: gishi523
 * Url: https://github.com/gishi523/kd-tree/blob/master/kdtree.h
 *
 * Created on November 19, 2017, 5:28 PM
 */

#ifndef KDTREE_H
#define KDTREE_H


#include <vector>
#include <numeric>
#include <algorithm>
#include <exception>
#include <functional>
#include <armadillo>

#include "GPGOMEA/Utils/Utils.h"


namespace kdt {

    /** @brief k-d tree class.
     */
    class KDTree {
    public:

        /** @brief The constructors.
         */
        KDTree();

        KDTree(const std::vector<arma::vec> &points);

        /** @brief The destructor.
         */
        ~KDTree();

        /** @brief Re-builds k-d tree.
         */
        void build(const std::vector<arma::vec> & points);

        void addPoint(arma::vec point);

        void deletePoint(arma::vec point);

        void build();

        /** @brief Clears k-d tree.
         */
        void clear();

        /** @brief Validates k-d tree.
         */
        bool validate();

        /** @brief Searches the nearest neighbor.
         */
        int nnSearch(const arma::vec& query, double_t* minDist = NULL, bool no_perfect_match = false);

        int nnSearch(const std::vector<arma::vec> & query, double_t * minDist = NULL, bool no_perfect_match = false);

        /** @brief Returns the points at the specified indices 
         */

        /*std::vector< arma::vec* > getPoints(const std::vector<int> & indices) {
            std::vector< std::vector <double_t > * > to_return;
            for (int i = 0; i < indices.size(); i++) {
                to_return.push_back(& points_[indices[i]]);
            }
            return to_return;
        }*/

        std::vector< arma::vec* > getPoints(const std::vector<int> & indices);

        size_t getPointsNumber();

        arma::vec * getPoint(int index);
        /** @brief Searches k-nearest neighbors.
         */
        std::vector<int> knnSearch(const arma::vec& query, int k);

        /** @brief Searches neighbors within radius.
         */
        std::vector<int> radiusSearch(const arma::vec& query, double_t radius);


    private:


        int _dim = 0;

        /** @brief k-d tree node.
         */
        struct Node {
            int idx; //!< index to the original point
            Node * next[2]; //!< pointers to the child nodes
            int axis; //!< dimension's axis

            Node() : idx(-1), axis(-1) {
                next[0] = next[1] = NULL;
            }

        };

        /** @brief k-d tree exception.
         */
        class Exception : public std::exception {
            using std::exception::exception;
        };

        /** @brief Bounded priority queue.
         */
        template <class T, class Compare = std::less<T>>
        class BoundedPriorityQueue {
        public:

            BoundedPriorityQueue() = delete;

            BoundedPriorityQueue(size_t bound) : bound_(bound) {
                elements_.reserve(bound + 1);
            };

            void push(const T& val) {
                auto it = std::find_if(std::begin(elements_), std::end(elements_),
                        [&](const T & element) {
                            return Compare()(val, element); });
                elements_.insert(it, val);

                if (elements_.size() > bound_)
                    elements_.resize(bound_);
            }

            const T& back() {
                return elements_.back();
            };

            const T& operator[](size_t index) {
                return elements_[index];
            }

            size_t size() {
                return elements_.size();
            }

        private:
            size_t bound_;
            std::vector<T> elements_;
        };

        /** @brief Priority queue of <distance, index> pair.
         */
        using KnnQueue = BoundedPriorityQueue<std::pair<double_t, int>>;

        /** @brief Builds k-d tree recursively.
         */
        Node* buildRecursive(int* indices, int npoints, int depth);

        /** @brief Clears k-d tree recursively.
         */
        void clearRecursive(Node* node);

        /** @brief Validates k-d tree recursively.
         */
        void validateRecursive(const Node* node, int depth);



        /** @brief Searches the nearest neighbor recursively.
         */
        void nnSearchRecursive(const arma::vec& query, const Node* node, int *guess, double_t *minDist, bool no_perfect_match = false);

        void nnSearchRecursive(const std::vector<arma::vec>& query, const Node* node, int *guess, double_t *minDist, bool no_perfect_match = false);

        /** @brief Searches k-nearest neighbors recursively.
         */
        void knnSearchRecursive(const arma::vec& query, const Node* node, KnnQueue& queue, int k);

        /** @brief Searches neighbors within radius.
         */
        void radiusSearchRecursive(const arma::vec& query, const Node* node, std::vector<int>& indices, double_t radius);

        Node* root_; //!< root node
        std::vector<arma::vec> points_; //!< points
    };
} // kdtS

#endif /* KDTREE_H */

