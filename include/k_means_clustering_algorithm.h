// Copyright Max Chetrusca, Oct 24 2014
// k_means_clustering_algorithm.h
// Object Clustering
// This file declares a class which uses OpenCV built-in K-Means Clustering
// algorithm together with a custom-implemented Elbow method to cluster a set of
// objects (represented by their images).

#ifndef OBJECT_CLUSTERING_K_MEANS_CLUSTERING_ALGORITHM_H_
#define OBJECT_CLUSTERING_K_MEANS_CLUSTERING_ALGORITHM_H_

#include <vector>

#include "abstract_cluster_algorithm.h"

namespace object_clustering {
// each object is characterized by 22 features
const int kNumberOfFeatures = 22;
// how many iterations per one cv::kmeans(..); call:
const int kNumberOfIterationsPerOneRun = 10;
// Usage:
// KMeansClusteringAlgorithm k;
// std::vector<Object> objects = ...;
// k.AssignGroupsToObjects(objects);
class KMeansClusteringAlgorithmTest;  // forward declaration for testing
class KMeansClusteringAlgorithm: public AbstractClusterAlgorithm {
  friend class KMeansClusteringAlgorithmTest;
 public:
  KMeansClusteringAlgorithm() = default;

  KMeansClusteringAlgorithm(const KMeansClusteringAlgorithm& algorithm) =
    default;

  KMeansClusteringAlgorithm& operator=(const KMeansClusteringAlgorithm&
    algorithm) = default;

  virtual ~KMeansClusteringAlgorithm() = default;
  // This method does the whole job:
  // after its call, object.group_ would be assigned
  // here we also define the abstract method from the base class:
  // objects should not be empty.
  int AssignGroupsToObjects(std::vector<Object> *objects) const override;

 private:
  // generates a set of unnormalized features which represent the mean color of
  // different regions of the image:
  std::vector<cv::Scalar> ColorFeaturesFromImage(const Image &image) const;
  // returns a vector of vectors of floats containing as many rows as examples,
  // each with kNumberOfFeatures columns, filled with scaled and normalized data
  // objects should not be empty.
  std::vector<std::vector<float>> AssignFeaturesFromObjects(
    const std::vector<Object> &objects) const;
  // bring the training_set numbers in the range (-1; 1):
  // training_set should not be empty.
  void NormalizeFeatures(std::vector<std::vector<float>> *training_set) const;
  // as we try to find optimal number of clusters, we need to compute the
  // error for each case:
  // clusters, training_set and centroids should not be empty;
  // num_of_training_examples and num_of_clusters should be >= 0;
  float ComputeError(
    const std::vector<int> &clusters,
    const std::vector<std::vector<float>> &training_set,
    const std::vector<std::vector<float>> &centroids,
    const int &num_of_training_examples,
    const int &num_of_clusters) const;
  // Clusters the training set with different random initial centroids then
  // chooses the best clustering and labels the objects accordingly.
  // training_set and objects should not be empty.
  int KMeansClusteringOpenCVImplementation(
    const std::vector<std::vector<float>> &training_set,
    std::vector<Object> *objects) const;
  // Assigns the best_labeling to objects:
  void LabelObjects(const std::vector<int> &best_labeling,
                    std::vector<Object> *objects) const;
};
}  // namespace object_clustering
#endif  // _OBJECT_CLUSTERING_K_MEANS_CLUSTERING_ALGORITHM_H_

