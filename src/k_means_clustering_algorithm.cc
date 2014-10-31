// Copyright Max Chetrusca, Oct 27 2014
// k_means_object_clustering_algorithm.cc
// Object Clustering

#include <cassert>
#include <cfloat>
#include <cstdlib>
#include <ctime>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "k_means_clustering_algorithm.h"
#include "gui_functions.h"

namespace object_clustering {
// Here are the features taken into consideration by clustering algorithm:
//    // Features:
//    // matrix.cols;
//    // matrix.rows;
// each mean color has 3 components:
//    // mean[0];
//    // mean[1];
//    // mean[2];
// 6 such "means" are taken into consideration:
// 1. the whole image;
// 2. the center of the image;
// 3,4,5,6. the corresponding subimages.
// abs(matrix.cols - matrix.rows);
// matrix.cols*matrix.cols;

int KMeansClusteringAlgorithm:: AssignGroupsToObjects(
    std::vector<Object> *objects) const {
  assert(objects != nullptr);
  assert(objects->size() > 0);
  // create the training set; extract the features:
  auto training_set = AssignFeaturesFromObjects(*objects);
  // perform the clustering:
  return KMeansClusteringOpenCVImplementation(training_set, objects);
}

std::vector<cv::Scalar> KMeansClusteringAlgorithm:: ColorFeaturesFromImage(
    const Image &image) const {
  cv::Mat matrix = image.matrix();
  std::vector<cv::Scalar> result;
  // the main color:
  result.push_back(mean(matrix));

  // the color of the center of the image:
  int x = matrix.cols * 0.2;
  int y = matrix.rows * 0.2;
  int width = matrix.cols - 2*x > 0 ? matrix.cols - 2*x : 1;
  int height = matrix.rows - 2*y > 0 ? matrix.rows - 2*y : 1;
  assert((x > 0) && (y > 0) && (width > 0) && (height > 0));
  cv::Mat inside_mat(matrix, cv::Rect(x, y, width, height));
  result.push_back(mean(inside_mat));
  // 4 subregions:
  width = matrix.cols;
  height = matrix.rows;
  cv::Mat m1(matrix, cv::Rect(0, 0, width/2, height/2));
  cv::Mat m2(matrix, cv::Rect(width/2, 0, width/2, height/2));
  cv::Mat m3(matrix, cv::Rect(0, height/2, width/2, height/2));
  cv::Mat m4(matrix, cv::Rect(width/2, height/2, width/2, height/2));
  result.push_back(mean(m1));
  result.push_back(mean(m2));
  result.push_back(mean(m3));
  result.push_back(mean(m4));

  return result;
}
// We just form a training_set of values gathered from the data contained in
// each object. These values are later normalized, so that each feature has the
// same weight.
std::vector<std::vector<float>> KMeansClusteringAlgorithm::
AssignFeaturesFromObjects(const std::vector<Object> &objects) const {
  assert(objects.size() > 0);
  int num_of_training_examples = static_cast<int>(objects.size());
  std::vector<std::vector<float>> training_set(num_of_training_examples);
  // Extract features:
  for (int i = 0; i < num_of_training_examples; i++) {
    Image image = objects[i].image();
    cv::Mat matrix = image.matrix();

    training_set[i].push_back(matrix.cols);  // width
    training_set[i].push_back(matrix.rows);  // height

    auto colors = ColorFeaturesFromImage(image);
    for (auto color : colors) {
      training_set[i].push_back(color[0]);  // avg blue
      training_set[i].push_back(color[1]);  // avg green
      training_set[i].push_back(color[2]);  // avg red
    }

    // how "square" is the image:
    training_set[i].push_back(abs(matrix.cols - matrix.rows));
    training_set[i].push_back(matrix.cols*matrix.cols);  // how big is the image
  }

  NormalizeFeatures(&training_set);

  return training_set;
}
// each feature of a training example has a value. Each feature has a maximal
// value and an average value across the training set. We first compute the
// maximal values and average values. To normalize a feature, we subtract avg
// from its value and divide by max value. In such a way we get a value
// between (-1; 1)
void KMeansClusteringAlgorithm:: NormalizeFeatures(
    std::vector<std::vector<float>> *training_set) const {
  assert(training_set != nullptr);
  assert(training_set->size() > 0);
  int num_of_training_examples = static_cast<int>(training_set->size());
  // Compute the avg and max:
  // For normalization and feature scaling:
  std::vector<float> max_feature_value(kNumberOfFeatures, -FLT_MAX);
  std::vector<float> avg_feature_value(kNumberOfFeatures, 0);

  for (int i = 0; i < num_of_training_examples; i++) {
    // Find max and avg feature values:
    for (int j = 0; j < kNumberOfFeatures; j++) {
      if (max_feature_value[j] < (*training_set)[i][j]) {
        max_feature_value[j] = (*training_set)[i][j];
      }
      avg_feature_value[j] += (*training_set)[i][j];
    }
  }
  for (auto& element : avg_feature_value) {
    element /= static_cast<float>(num_of_training_examples);
  }
  // Normalize features using max and avg feature values:
  for (int i = 0; i < num_of_training_examples; i++) {
    for (int j = 0; j < kNumberOfFeatures; j++) {
      if (max_feature_value[j] == 0) {
        (*training_set)[i][j] = 0.99;
      } else {
        (*training_set)[i][j] =
        ((*training_set)[i][j] - avg_feature_value[j]) /
                                 max_feature_value[j];
      }

      // This should not happen:
      assert(((*training_set)[i][j] > -1) && ((*training_set)[i][j] < 1));
    }
  }
}
// We compute the error using the Euclidean distance formula - the difference
// between the exemples assigned to a centroid and the centroid itself.
float KMeansClusteringAlgorithm:: ComputeError(
    const std::vector<int> &clusters,
    const std::vector<std::vector<float>> &training_set,
    const std::vector<std::vector<float>> &centroids,
    const int &n,  // num_of_training_examples
    const int &K) const {  // K - num_of_clusters
  assert(n >= 0);
  assert(K >= 0);
  assert(clusters.size() > 0);
  assert(training_set.size() > 0);
  assert(centroids.size() > 0);
  float error = 0;
  for (int i = 0 ; i < n; i++) {
    for (int j = 0; j < K; j++) {
      if (clusters[i] == j) {
        std::vector<float> v;
        for (int l = 0; l < kNumberOfFeatures; l++) {
          float a = training_set[i][l] - centroids[j][l];
          v.push_back(a);
        }
        error += cv::norm(v);
      }
    }
  }
  error /= n;
  assert(error >= 0);
  return error;
}

int KMeansClusteringAlgorithm:: KMeansClusteringOpenCVImplementation(
    const std::vector<std::vector<float>> &training_set,
    std::vector<Object> *objects) const {
  assert(training_set.size() > 0);
  assert(objects != nullptr);
  assert(objects->size() > 0);
  // 1. Prepare the data:
  int num_of_training_examples = static_cast<int>(objects->size());
  std::vector<int> best_labeling(num_of_training_examples);
  // the computed error cannot be negative. this assignment is to show that
  // there is no previous_error:
  float previous_error = -1;
  float previous_error_ratio = 1;
  int resulting_num_of_clusters = 1;
  // 1.2 Create an input matrix of data for kmeans() function:
  cv::Mat data(num_of_training_examples, kNumberOfFeatures, CV_32FC1);
  for (int i = 0; i < num_of_training_examples; i++) {
    for (int j = 0; j < kNumberOfFeatures; j++) {
      data.at<float>(i, j) = training_set[i][j];
    }
  }

  // 2. We iteratively try to group objects in different number of groups.
  // By Elbow method, the error decreases as the number of clusters increases.
  // At some point, the slope of this decrease falls down - that is, the error
  // met the elbow - we should stop here. This is considered the optimal number
  // of clusters.
  // kmeans provided by OpenCV does not automatically determine the needed
  // number of clusters, that is why Elbow method is used.
  // So we iteratively run kmeans, compute the error, compare it with
  // previous_error and decide whether to stop.
  for (int num_of_clusters = 1;
       num_of_clusters <= num_of_training_examples;
       num_of_clusters++) {
    // 2.1 Some setup before we run kmeans:
    cv::Mat labels;
    cv::TermCriteria criteria =
    cv::TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0);
    int attempts = kNumberOfIterationsPerOneRun;
    int flags = cv::KMEANS_PP_CENTERS;
    cv::Mat centers(num_of_clusters, 1, data.type());
    // 2.2 OpenCV kmeans: finds centers of clusters and groups the input samples
    // around the clusters.
    cv::kmeans(data,
               num_of_clusters,
               labels,
               criteria,
               attempts,
               flags,
               centers);
    if (num_of_training_examples == 1) {
      assert(labels.at<int>(0) == 0);
      // Strange enough, when there is just one object,
      // the group assigned to it is 8;
      // Let's correct that:
      // labels.at<int>(0) = 0;
    }
    // 2.3 Copy the output labels into a vector:
    std::vector<int> clusters(num_of_training_examples);

    for (int i = 0; i < num_of_training_examples; i++) {
      clusters[i] = labels.at<int>(i);
    }
    // 2.4 Similar here:
    std::vector<std::vector<float>> centroids(num_of_clusters);
    for (int i = 0; i < num_of_clusters; i++) {
      centroids[i] = std::vector<float>(kNumberOfFeatures);
      for (int j = 0; j < kNumberOfFeatures; j++) {
        centroids[i][j] = centers.at<float>(i, j);
      }
    }
    // 2.5 Compute error and check if it is time to stop:
    float error = ComputeError(clusters,
                               training_set,
                               centroids,
                               num_of_training_examples,
                               num_of_clusters);

    if (error == 0) {
      resulting_num_of_clusters = num_of_clusters;
      for (int i = 0; i < num_of_training_examples; i++) {
        best_labeling[i] = labels.at<int>(i);
      }

      break;
    }
    // if this is the first time:
    if (previous_error < 0) previous_error = error;
    // "Elbow" method: the error is going down slowly:
    if (previous_error_ratio > previous_error/error) {
      break;
    } else {
      resulting_num_of_clusters = num_of_clusters;
      previous_error_ratio = previous_error/error;
      for (int i = 0; i < num_of_training_examples; i++) {
        best_labeling[i] = labels.at<int>(i);
      }
    }
    previous_error = error;
  }
  // 2.6 Label the objects:
  LabelObjects(best_labeling, objects);

  return resulting_num_of_clusters;
}

void KMeansClusteringAlgorithm:: LabelObjects(
    const std::vector<int> &best_labeling,
    std::vector<Object> *objects) const {
  int i = 0;
  for (auto &obj : *objects) {
     obj.set_group(best_labeling[i]);
     // std::cout << i << " " << best_labeling[i] << std::endl;
     i++;
  }
}

}  // namespace object_clustering

