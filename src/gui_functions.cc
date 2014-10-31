// Copyright Max Chetrusca, Oct 27 2014
// gui_functions.cc
// Object Clustering

#include <cassert>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

#include "image.h"
#include "object.h"
#include "gui_functions.h"

namespace object_clustering {
void ShowImage(const Image &image) {
  cv::imshow("one", image.matrix());
  cv::waitKey();
  cv::destroyWindow("one");
}

// Displays a window on the screen, showing the result of clustering
void ShowResult(const Image &image,
                const std::vector<Object> &objects,
                const int &num_of_groups) {
  assert(objects.size() > 0);
  assert(num_of_groups > 0);
  cv::Mat image_matrix = image.matrix();
  for (int i = 0; i < num_of_groups; i++) {
    cv::Scalar color = cvScalar(rand() % 255, rand() % 255, rand() % 255);
    for (int j = 0; j < objects.size(); j++) {
      if (objects[j].group() == i) {
        rectangle(image_matrix,
                  objects[j].image().bounding_rect().tl(),
                  objects[j].image().bounding_rect().br(),
                  color,
                  5,
                  8,
                  0);
      }
    }
  }
  ShowImage(Image(image_matrix));
}
}  // namespace object_clustering

