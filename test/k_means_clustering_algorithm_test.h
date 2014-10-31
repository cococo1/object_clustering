// Copyright Max Chetrusca Oct 30 2014
// k_means_clustering_algorithm_test.h
// Object clustering
// A friend test-class for KMeansClusteringAlgorithm class.
#ifndef OBJECT_CLUSTERING_K_MEANS_CLUSTERING_ALGORITHM_TEST_H_
#define OBJECT_CLUSTERING_K_MEANS_CLUSTERING_ALGORITHM_TEST_H_

#include "gui_functions.h"
#include "image.h"
#include "k_means_clustering_algorithm.h"
#include "object_detector.h"

namespace object_clustering {
class KMeansClusteringAlgorithmTest {
 public:
  static bool TestKMeansClusteringAlgorithm() {
    KMeansClusteringAlgorithmTest k;
    return //k.TestAssignGroupsToObjects() &&
           //k.TestAssignFeatures() &&
           //k.TestNormalizeFeatures();
           //k.TestComputeError();
           k.TestKMeans();
  }
  bool TestAssignGroupsToObjects() {
    ObjectDetector d;
    KMeansClusteringAlgorithm k;
    for (int i = 1; i <= 17; i++) {
      Image img("images/" + std::to_string(i) + "-2.png");
      Image background("images/" + std::to_string(i) + "-1.png");
      ShowImage(img);
      auto v = d.DetectObjectsFromImage(img, background);
      int n = k.AssignGroupsToObjects(&v);
      ShowResult(img, v, n);
    }
    // should give errors:
    //k.AssignGroupsToObjects(nullptr);
    std::vector<Object> v;
    //k.AssignGroupsToObjects(&v);
    return true;
  }
  bool TestAssignFeatures() {
     ObjectDetector d;
     KMeansClusteringAlgorithm k;
     for (int i = 1; i <= 17; i++) {
      Image img("images/" + std::to_string(i) + "-2.png");
      Image background("images/" + std::to_string(i) + "-1.png");
      ShowImage(img);
      auto v = d.DetectObjectsFromImage(img, background);
      k.AssignFeaturesFromObjects(v);
      //auto v = d.NormalizeFeatures();
      //int n = k.AssignGroupsToObjects(&v);
    }
    // should give an error:
    std::vector<Object> v;
    //k.AssignFeaturesFromObjects(v);
    return true;
  }
  bool TestNormalizeFeatures() {
    ObjectDetector d;
    KMeansClusteringAlgorithm k;
    // should give an error:
    //k.NormalizeFeatures(nullptr);
    std::vector<std::vector<float>> t;
    //k.NormalizeFeatures(&t);
    return true;
  }
  bool TestComputeError() {
    KMeansClusteringAlgorithm k;
    // should give an error:
    std::vector<int> clusters;
    std::vector<std::vector<float>> training_set;
    std::vector<std::vector<float>> centroids;
    int n = -1;
    int K = -1;

    //k.ComputeError(clusters, training_set, centroids, n, K);
    return true;
  }
  bool TestKMeans() {
    KMeansClusteringAlgorithm k;
    // should give an error:
    std::vector<std::vector<float>> training_set;
    //k.KMeansClusteringOpenCVImplementation(training_set, nullptr);
    std::vector<Object> objects;
    //k.KMeansClusteringOpenCVImplementation(training_set, &objects);
    

    return true;
  }
};
}  // namespace object_clustering

#endif  // OBJECT_CLUSTERING_K_MEANS_CLUSTERING_ALGORITHM_TEST_H_

