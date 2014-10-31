// Copyright Max Chetrusca, Oct 23 2014
// gui_functions.h
// Some GUI helper functions
#ifndef OBJECT_CLUSTERING_GUI_FUNCTIONS_H_
#define OBJECT_CLUSTERING_GUI_FUNCTIONS_H_

#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

#include "image.h"
#include "object.h"

namespace object_clustering {
void ShowImage(const Image &image);
// Displays a window on the screen, showing the result of clustering
// objects should not be empty;
// num_of_groups should be > 0;
void ShowResult(const Image &image,
                const std::vector<Object> &objects,
                const int &num_of_groups);
}  // namespace object_clustering
#endif  //  OBJECT_CLUSTERING_GUI_FUNCTIONS_H_

