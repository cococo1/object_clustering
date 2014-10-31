// Copyright Max Chetrusca, Oct 23 2014
// image.h
// Object clustering
// Declares a class which wraps an OpenCV image, adding a few additional
// properties

#ifndef OBJECT_CLUSTERING_IMAGE_H_
#define OBJECT_CLUSTERING_IMAGE_H_

#include <cstdio>

#include <algorithm>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

// All types and functions defined during this project go into this namespace:
namespace object_clustering {

const int kImageWidth = 1280;  // in pixels
const int kImageHeight = 960;

// Used to identify an image.
// The bounded_rect() and GetCenter() methods return the position of the image
// in the superimage (image that contains this image)
// Usage:
// object_clutering::Image i("/path/to/image.png");
// auto center = i.center();
// ...
class Image {
 public:
  // An image without a matrix is not an image:
  Image() = delete;
  // An image is loaded from the file. In this case, the superimage is the image
  // itself, so bounding_rect is set the upper-left corner.
  explicit Image(const std::string &filename):
    matrix_(cv::imread(filename, CV_LOAD_IMAGE_COLOR)),
    bounding_rect_(cv::Rect(0, 0, kImageWidth, kImageHeight)) {
    if (matrix_.data == NULL) {
      fprintf(stderr, "Could not read the image from file %s \n",
      filename.c_str());
      exit(1);
    }
  }
  // An image is constructed from the matrix. No bounding rect is specified,
  // thus it occupies the whole space:
  explicit Image(const cv::Mat &matrix):
     bounding_rect_(cv::Rect(0, 0, kImageWidth, kImageHeight)) {
     matrix.copyTo(matrix_);
  }
  // An image is constructed from a matrix and the position of this matrix in
  // the superimage:
  explicit Image(const cv::Mat &matrix, const cv::Rect &bounding_rect):
    bounding_rect_(bounding_rect) {
    matrix.copyTo(matrix_);
  }
  // A copy of an image is independent of the original:
  Image(const Image &image):
    bounding_rect_(image.bounding_rect_) {
    image.matrix_.copyTo(matrix_);
  }
  // Implemented using copy-and-swap idiom:
  Image& operator=(Image image) {
    std::swap(matrix_, image.matrix_);
    std::swap(bounding_rect_, image.bounding_rect_);
    return *this;
  }
  // no dynamic memory to manage:
  virtual ~Image() = default;

  cv::Mat matrix() const { return matrix_; }

  cv::Rect bounding_rect() const { return bounding_rect_; }

  // Returns the center of the image in its superimage coordinates:
  cv::Point2d GetCenter() const {
    return cv::Point2d(bounding_rect_.x + bounding_rect_.width/2,
                       bounding_rect_.y + bounding_rect_.height/2);
  }

  void set_bounding_rect(const cv::Rect &bounding_rect) {
    bounding_rect_ = bounding_rect;
  }

 private:
  // matrix of pixels:
  cv::Mat matrix_;
  // the rectangle which bounds this image in its superimage:
  cv::Rect bounding_rect_;
};
}  // namespace object_clustering
#endif  // OBJECT_CLUSTERING_IMAGE_H_

