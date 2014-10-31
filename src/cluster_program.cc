// Copyright Max Chetrusca, Oct 23 2014
// main.cc
// Object Clustering
// A clustering application. Given two images: one of the background and the
// other of the objects, the program detects and circles the objects, each
// group with a different color.
// Usage: cluster background_image object_image

#include <cstdio>

#include "gui_functions.h"
#include "object_detector.h"
#include "k_means_clustering_algorithm.h"

namespace oc = object_clustering;

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: cluster background_image object_image \n");
    std::exit(1);
  }
  auto image_name = argv[2];
  auto background_name = argv[1];
  // 0.1 Extract images:
  oc::Image objects_image(image_name);
  oc::Image background(background_name);
  // 1. Detect objects;
  oc::ObjectDetector object_detector;
  auto objects = object_detector.DetectObjectsFromImage(objects_image,
                                                        background);
  // 2. Cluster them;
  oc::KMeansClusteringAlgorithm object_clusterer;
  auto num_of_groups = object_clusterer.AssignGroupsToObjects(&objects);
  // 3. Show the result.
  oc::ShowResult(objects_image, objects, num_of_groups);
  return 0;
}

