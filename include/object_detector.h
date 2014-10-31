// Copyright Max Chetrusca, Oct 24 2014
// object_detector.h
// Object Clustering
// Declares a class used to detect objects in a image, given a clear background.

#ifndef OBJECT_CLUSTERING_OBJECT_DETECTOR_H_
#define OBJECT_CLUSTERING_OBJECT_DETECTOR_H_

#include <vector>

#include "object.h"

namespace object_clustering {
const float kMinimalAreaForObjectIdentification = 2000;  // pixels
const float kMaximalAreaForObjectIdentification = 50000;
// Detects the objects from the image.
// Usage:
// object_clustering::Image background = ...;
// object_clustering::Image image = ...;
// object_clustering::ObjectDetector detector;
// auto objects = detector.DetectObjectsFromImage(image, background);
class ObjectDetectorTest;  // forward declaration for testing
class ObjectDetector {
  friend class ObjectDetectorTest;
 public:
  ObjectDetector() = default;
  // No need to copy a detector:
  ObjectDetector(const ObjectDetector &detector) = delete;

  ObjectDetector& operator=(const Object &detector) = delete;

  virtual ~ObjectDetector() = default;
  // Takes two images of the same size. One image is a clear background, like a
  // plain floor, and on the other the objects are present.
  // Returns a vector of detected objects.
  std::vector<Object> DetectObjectsFromImage(const Image &image,
                                             const Image &background) const;

 private:
  // Returns true if the rect rectangles[index] has its center inside of any of
  // the other rectangles:
  // rectangles should not be empty;
  bool RectCenterInsideOtherRect(const int &index,
                               const cv::vector<cv::Rect> &rectangles) const;
  // This method subtracts the two given images, returning a mask containing
  // only black & white pixels, denoting the objects. Shadow is also eliminated
  // here.
  // images should be of the same size.
  cv::Mat ComputeForegroundMask(const Image &image,
                                const Image &background) const;
  // Simple transformation which makes all exactly black pixels (0,0,0) ->
  // white and all the other black.
  // mat should not be NULL.
  void RecolorDetectedPixels(cv::Mat *mat) const;
  // Given the image and the background, returns a grayed matrix of black &
  // white pixels after background subtraction & recoloring:
  cv::Mat ExtractForegroundAndPreprocess(const Image &image,
                                         const Image &background) const;
  // Fills in the detected rectangles of objects from the given gray matrix:
  // threshold_output and good_rects should not be NULL.
  void DetectBoundingRectsAndEdges(const cv::Mat &src_gray,
                                   cv::Mat *threshold_output,
                                   cv::vector<cv::Rect> *good_rects) const;
  // Detects the contours of the objects from the gray image. Determines also
  // the threshold which gives the most contours. The contours which are either
  // too small or too big are ignored.
  // best_contours and threshold_output should not be NULL.
  void DetectContoursInMatrixWithThresholdOutput(const cv::Mat &gray,
                              cv::vector<cv::vector<cv::Point>> *best_contours,
                              cv::Mat *threshold_output) const;
  // Finds the enclosing rectangles for the given contours:
  // contours should not be empty;
  // good_rects should not be NULL.
  void GetGoodBoundingRectsOfContours(
      const cv::vector<cv::vector<cv::Point>> &contours,
      cv::vector<cv::Rect> *good_rects) const;
  // Given the initial image (src) the method creates the objects from the
  // rectangles that have been found.
  // good_rects should not be empty.
  std::vector<Object> GetObjectsFromRects(
      const cv::vector<cv::Rect> &good_rects,
      const cv::Mat &threshold_output,
      const cv::Mat &src) const;
};
}  // namespace object_clustering
#endif  // OBJECT_CLUSERING_OBJECT_DETECTOR_H_

