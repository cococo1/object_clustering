// Copyright Max Chetrusca Oct 24, 2014
// test.cc
// Object Clusterer
// Tests for created classes and functions

#include <cstdio>
#include "image_test.h"
#include "object_test.h"
#include "object_detector_test.h"
#include "k_means_clustering_algorithm_test.h"

int main() {
  //object_clustering::ImageTest::TestImage();
  //object_clustering::ObjectTest::TestObject();
  //object_clustering::ObjectDetectorTest::TestObjectDetector();
  object_clustering::KMeansClusteringAlgorithmTest::
                     TestKMeansClusteringAlgorithm();
  printf("All tests passed. \n");
  return 0;
}

