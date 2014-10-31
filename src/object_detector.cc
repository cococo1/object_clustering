// Copyright Max Chetrusca, Oct 24 2014
// object_detector.cc
// Object Clustering

#include <cassert>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/opencv.hpp"

#include "object_detector.h"

namespace object_clustering {
// This method:
// 1. Extracts background and preprocesses the image;
// 2. Detects contours of the objecst then approximates them to rects;
// 3. Create "objects" from those rects.
std::vector<Object> ObjectDetector::DetectObjectsFromImage(
    const Image &image,
    const Image &background) const {
  // image and background should have the same size:
  assert(image.matrix().rows == background.matrix().rows);
  assert(image.matrix().cols == background.matrix().cols);
  // 1:
  cv::Mat src_gray;
  ExtractForegroundAndPreprocess(image, background).copyTo(src_gray);
  // 2:
  // Detect contours/edges using Threshold;
  // Approximate contours to polygons + get bounding rects:
  cv::Mat threshold_output;
  cv::vector<cv::Rect> good_rects;
  DetectBoundingRectsAndEdges(src_gray, &threshold_output, &good_rects);
  // 3:
  // Create the objects from those rects:
  auto src = image.matrix();
  std::vector<Object> detected_objects;
  return GetObjectsFromRects(good_rects,
                             threshold_output,
                             src);
}

bool ObjectDetector::RectCenterInsideOtherRect(
    const int &index,
    const cv::vector<cv::Rect> &rectangles) const {
  assert(rectangles.size() > 0);
  assert(index >= 0);
  assert(index < rectangles.size());
  bool result = false;
  auto current_rect = rectangles[index];
  cv::Point2d center(current_rect.x + current_rect.width/2,
                     current_rect.y + current_rect.height/2);
  for (int i = 0; i < rectangles.size(); i++) {
      if (i != index) {
          if (rectangles[i].contains(center) &&
              (current_rect.area() < rectangles[i].area())) {
              result = true;
              break;
          }
      }
  }
  return result;
}
// The mask is computed using the OpenCV BackgroundSubtractorMOG2 class.
// It detects the shadows as well
cv::Mat ObjectDetector::ComputeForegroundMask(
    const Image &image,
    const Image &background) const {
  assert(image.matrix().rows == background.matrix().rows);
  assert(image.matrix().cols == background.matrix().cols);
  // 1. Setup the subtractor:
  cv::Mat mask;
  int history = 2;
  float var_threshold = 50;
  bool shadow_detection = true;
  cv::Ptr<cv::BackgroundSubtractor> subtractor =
  new cv::BackgroundSubtractorMOG2(history,
                                   var_threshold,
                                   shadow_detection);
  subtractor->set("fVarInit", 100);
  subtractor->set("fTau", 0.05);
  subtractor->operator()(background.matrix(), mask);
  subtractor->operator()(image.matrix(), mask);
  subtractor.release();
 // return mask;
  // 2. Recolor the shadow to 0 (consider it as background):
  // the subtractor recolors the shadow into 127 intensity;
  // so to eliminate the shadow, just recolor these pixels:
  for (int i = 0; i < mask.rows; i++) {
      for (int j = 0; j < mask.cols; j++) {
          if (mask.at<uchar>(i, j) == 127) {
              mask.at<uchar>(i, j) = 0;
          }
      }
  }

  return mask;
}

void ObjectDetector::RecolorDetectedPixels(cv::Mat *mat) const {
  assert(mat != nullptr);
  for (int y = 0; y < mat->rows; y++) {
      for (int x = 0; x < mat->cols; x++) {
          cv::Vec3b pixel = mat->at<cv::Vec3b>(y, x);
          if ((pixel.val[0] == 0) &&
              (pixel.val[1] == 0) &&
              (pixel.val[2] == 0)) {
              pixel.val[0] = 255;
              pixel.val[1] = 255;
              pixel.val[2] = 255;
              mat->at<cv::Vec3b>(y, x) = pixel;
          } else {
              pixel.val[0] = 0;
              pixel.val[1] = 0;
              pixel.val[2] = 0;
              mat->at<cv::Vec3b>(y, x) = pixel;
            }
      }
  }
}
// Preprocessing done using the methods described above.
cv::Mat ObjectDetector::ExtractForegroundAndPreprocess(
    const Image &image,
    const Image &background) const {
  cv::Mat mask;
  ComputeForegroundMask(image, background).copyTo(mask);

  auto src = image.matrix();
  cv::Mat recolored_src;
  src.copyTo(recolored_src, mask);  // copy using mask.

  //  Recolor the detected pixels:
  RecolorDetectedPixels(&recolored_src);

  cv::Mat src_gray;
  // Convert image to gray and blur it
  cvtColor(recolored_src, src_gray, CV_BGR2GRAY);
  blur(src_gray,
       src_gray,
       cv::Size(3, 3));

  return src_gray;
}

void ObjectDetector::DetectBoundingRectsAndEdges(
    const cv::Mat &src_gray,
    cv::Mat *threshold_output,
    cv::vector<cv::Rect> *good_rects) const {
    assert(threshold_output != nullptr);
    assert(good_rects != nullptr);
    // Detect contours/edges using Threshold
    cv::vector<cv::vector<cv::Point>> best_contours;
    DetectContoursInMatrixWithThresholdOutput(src_gray,
                                              &best_contours,
                                              threshold_output);

  cv::vector<cv::vector<cv::Point>> contours;
  contours = best_contours;

  // Approximate contours to polygons + get bounding rects:
  GetGoodBoundingRectsOfContours(contours, good_rects);
}

// Using the functionality form OpenCV, we can find contours adjusting different
// threshold. We try every possible threshold and select the one which gives the
// most contours which pass the area conditions - they are neither too small nor
// too big.
void ObjectDetector::DetectContoursInMatrixWithThresholdOutput(
    const cv::Mat &gray,
    cv::vector<cv::vector<cv::Point>> *best_contours,
    cv::Mat *threshold_output) const {
  assert(best_contours != nullptr);
  assert(threshold_output != nullptr);
  cv::vector<cv::Vec4i> hierarchy;
  int max_num_of_contours = 0;
  for (int i = 0; i < 256; i++) {
    // applies a fixed-level threshold i to each gray element:
    threshold(gray, *threshold_output, i, 255, cv::THRESH_BINARY);
    cv::vector<cv::vector<cv::Point>> contours;
    cv::vector<cv::vector<cv::Point>> candidate_contours;
    // finds contours in a binary image;
    // here threshold_output is the input image.
    findContours(*threshold_output,
                 contours,
                 hierarchy,
                 CV_RETR_TREE,
                 CV_CHAIN_APPROX_SIMPLE,
                 cv::Point(0, 0));

    int current_num_of_contours = 0;
    for (int j = 0; j < contours.size(); j++) {
      float area = contourArea(contours[j]);
      if ((area > kMinimalAreaForObjectIdentification) &&
          (area < kMaximalAreaForObjectIdentification)) {
        current_num_of_contours++;
        candidate_contours.push_back(contours[j]);
      }
    }
    if (current_num_of_contours > max_num_of_contours) {
      max_num_of_contours = current_num_of_contours;
      *best_contours = candidate_contours;
    }
  }
}
// Approximates contours to polygons, polygons to other polygons with less
// vertices, then finally generates rectangles each of which encloses a set of
// points (a polygon). From those rectangles only the ones with good size are
// taken.
void ObjectDetector::GetGoodBoundingRectsOfContours(
    const cv::vector<cv::vector<cv::Point>> &contours,
    cv::vector<cv::Rect> *good_rects) const {
  assert(contours.size() > 0);
  assert(good_rects != nullptr);
  // Approximate contours to polygons + get bounding rects
  cv::vector<cv::vector<cv::Point>> contours_poly(contours.size());
  cv::vector<cv::Rect> bound_rect(contours.size());

  for (int i = 0; i < contours.size(); i++) {
    // approximate a curve/poly to another curve/poly with less vertices:
    approxPolyDP(cv::Mat(contours[i]),
                 contours_poly[i],
                 3,
                 true);
    // computes the bounding rect for a set of points:
    bound_rect[i] = boundingRect(cv::Mat(contours_poly[i]));
  }
  // check the size:
  for (int i = 0; i < contours.size(); i++) {
    float area = contourArea(contours[i]);
    if ((area > kMinimalAreaForObjectIdentification) &&
        (area < kMaximalAreaForObjectIdentification)) {
      good_rects->push_back(bound_rect[i]);
    }
  }
}
// "Cut" the rectangles from the original image and pass them as images to
// Object class constructors, thus creating objects.
std::vector<Object> ObjectDetector::GetObjectsFromRects(
  const cv::vector<cv::Rect> &good_rects,
  const cv::Mat &threshold_output,
  const cv::Mat &src) const {
  assert(good_rects.size() > 0);
  std::vector<Object> detected_objects;
  for (int i = 0; i < good_rects.size(); i++) {
    if (!RectCenterInsideOtherRect(i, good_rects)) {
     // Create an object out of this:
      cv::Mat object_mat;
      src(good_rects[i]).copyTo(object_mat);
      Image object_image(object_mat);
      object_image.set_bounding_rect(good_rects[i]);
      Object detected_object(object_image);
      detected_objects.push_back(detected_object);
    }
  }
  return detected_objects;
}

}  // namespace object_clustering

