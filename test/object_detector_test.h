// Copyright Max Chetrusca, Oct 28 2014
// object_detector_test.h
// Object clustering
// A friend-test class for ObjectDetector class.
#ifndef OBJECT_CLUSTERING_OBJECT_DETECTOR_TEST_H_
#define OBJECT_CLUSTERING_OBJECT_DETECTOR_TEST_H_

#include <cassert>

#include <string>
#include <vector>

#include "opencv2/imgproc/imgproc.hpp"

#include "gui_functions.h"
#include "image.h"
#include "object_detector.h"

namespace object_clustering {
class ObjectDetectorTest {
 public:
  static bool TestObjectDetector() {
    ObjectDetectorTest obj_detector_test;
    return obj_detector_test.TestCreation() &&
           //obj_detector_test.TestGetObjects() &&
           //obj_detector_test.TestGetGoodRects() &&
           //obj_detector_test.TestDetectContours() &&
           obj_detector_test.TestRectCenterInsideOtherRect() &&
           //obj_detector_test.TestRecolorDetectedPixels() &&
           obj_detector_test.TestExtractForeground() &&
           obj_detector_test.TestComputeForegroundMask() &&
           obj_detector_test.TestDetectObjects(); 
           
  }
  bool TestCreation() {
    ObjectDetector o;
    //ObjectDetector o2(o); // should not compile
    ObjectDetector o3;
    //o3 = o; // should not compile
    return true;
  }
  void ShowObjects(const std::vector<Object> &objects) {
    for(auto obj : objects) {
      ShowImage(obj.image());
    }
  }
  bool TestDetectObjects() {
    ObjectDetector detector;
    for (int i = 1; i <= 17; i++) {
      Image img("images/" + std::to_string(i) + "-2.png");
      Image background("images/" + std::to_string(i) + "-1.png");
      ShowImage(img);
      auto v = detector.DetectObjectsFromImage(img, background);
      //assert(v.size() == 12);
      ShowObjects(v);
    }
   
    return true;
  }
  bool TestRectCenterInsideOtherRect() {
    cv::Rect r(0, 0, 1000, 1000);
    cv::Rect r2(100, 100, 500, 500);
    ObjectDetector d;
    assert(d.RectCenterInsideOtherRect(1,{r, r2}));
    cv::Rect r3(0, 0, 200, 400);
    cv::Rect r4(600, 0 , 30, 40);
    assert(! d.RectCenterInsideOtherRect(1, {r3, r4}));
    cv::Rect r5(10, 10, 100, 100);
    cv::Rect r6(90, 90, 200, 200);
    assert(! d.RectCenterInsideOtherRect(1, {r5, r6}));
    cv::Rect r7(-50, -50, 100, 100);
    cv::Rect r8(-1, -1, 80, 80);
    assert(d.RectCenterInsideOtherRect(1, {r7, r8}));
    cv::Rect r9(-100, -100, 300, 300);
    assert(! d.RectCenterInsideOtherRect(2, {r7, r8, r9}));
    // should give a runtime error:
    //assert(! d.RectCenterInsideOtherRect(1, {}));  
    return true;
  }
  bool TestComputeForegroundMask() {
    ObjectDetector d;
    for (int i = 1; i <= 17; i++) {
      Image img("images/" + std::to_string(i) + "-2.png");
      Image background("images/" + std::to_string(i) + "-1.png");
      Image result(d.ComputeForegroundMask(img, background));
      //ShowImage(background);
      //ShowImage(img);
      //ShowImage(result);
    }
    return true; 
    Image i("images/2-2.png");
    Image b("images/2-1.png");
    Image result(d.ComputeForegroundMask(i, b));
    ShowImage(result);
    Image result2(d.ComputeForegroundMask(i, i));
    ShowImage(result2);
    Image white("images/white.png");
    Image black("images/black.png");
    Image result3(d.ComputeForegroundMask(white, black));
    ShowImage(result3);
    //d.ComputeForegroundMask(i, white); // should give a runtime error
    
    return true;
  }
  bool TestRecolorDetectedPixels() {
    Image i("images/7-2.png");
    Image b("images/7-1.png");
    ObjectDetector d;
    Image result(d.ComputeForegroundMask(i, b));
    ShowImage(result);
    cv::Mat m = result.matrix();
    d.RecolorDetectedPixels(&m);
    ShowImage(result);
    //d.RecolorDetectedPixels(nullptr);  // should give an error
    return true;
  }
  bool TestExtractForeground() {
    ObjectDetector d;
    Image i("images/7-2.png");
    Image b("images/7-1.png");
    auto m = d.ExtractForegroundAndPreprocess(i, b);
    ShowImage(Image(m));
    return true;
  }
  bool TestDetectBoundingRects() {
    ObjectDetector d;
    cv::Mat m;
    cv::vector<cv::Rect> rects;
    d.DetectBoundingRectsAndEdges(m, nullptr, &rects);  // should give an error
    d.DetectBoundingRectsAndEdges(m, &m, nullptr);  // should give an error
    return true;
  }
  bool TestDetectContours() {
    ObjectDetector d;
    for (int i = 1; i <= 17; i++) {
      Image img("images/" + std::to_string(i) + "-2.png");
      Image background("images/" + std::to_string(i) + "-1.png");
      ShowImage(img);
      auto m = d.ExtractForegroundAndPreprocess(img, background);
      cv::vector<cv::vector<cv::Point>> contours;
      cv::Mat threshold;
      d.DetectContoursInMatrixWithThresholdOutput(m, &contours, &threshold);
    
      cv::Mat drawing = cv::Mat::zeros(threshold.size(), CV_8UC3);
      cv::RNG rng(12345);
      cv::vector<cv::Vec4i> hierarchy;
      for(int i = 0; i < contours.size(); i++) {
       cv::Scalar color = cv::Scalar(rng.uniform(0, 255),
                                     rng.uniform(0,255),
                                     rng.uniform(0,255));
        drawContours(drawing,
                     contours,
                     i,
                    color,
                    2,
                    8,
                    hierarchy,
                    0,
                    cv::Point());
      }
      ShowImage(Image(drawing));     
    }
    cv::Mat m;
    // should give an error:
    //d.DetectContoursInMatrixWithThresholdOutput(m, nullptr, &m);
    // should also give an error:
    cv::vector<cv::vector<cv::Point>> contours;
    //d.DetectContoursInMatrixWithThresholdOutput(m, &contours, nullptr);
    return true;
  }
  bool TestGetGoodRects() {
    ObjectDetector d;
    for (int i = 1; i <= 17; i++) {
      Image img("images/" + std::to_string(i) + "-2.png");
      Image background("images/" + std::to_string(i) + "-1.png");
      ShowImage(img);
      auto m = d.ExtractForegroundAndPreprocess(img, background);
      cv::vector<cv::vector<cv::Point>> contours;
      cv::Mat threshold;
      d.DetectContoursInMatrixWithThresholdOutput(m, &contours, &threshold);
      cv::vector<cv::Rect> good_rects;
      d.GetGoodBoundingRectsOfContours(contours, &good_rects);
      
      cv::Mat drawing = cv::Mat::zeros(threshold.size(), CV_8UC3);
      cv::RNG rng(12345);
      for(int i = 0; i< contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(0, 255),
                                      rng.uniform(0,255),
                                      rng.uniform(0,255));

        rectangle(drawing,
                  good_rects[i].tl(),
                  good_rects[i].br(),
                  color,
                  2,
                  8,
                  0);
      }
      ShowImage(Image(drawing));
    }
    // should give an error:
    cv::vector<cv::vector<cv::Point>> contours;
    //d.GetGoodBoundingRectsOfContours(contours, nullptr);
    return true;
  }
  bool TestGetObjects() {
    ObjectDetector d;
    for (int i = 1; i <= 17; i++) {
      Image img("images/" + std::to_string(i) + "-2.png");
      Image background("images/" + std::to_string(i) + "-1.png");
      ShowImage(img);
      auto m = d.ExtractForegroundAndPreprocess(img, background);
      cv::vector<cv::vector<cv::Point>> contours;
      cv::Mat threshold;
      d.DetectContoursInMatrixWithThresholdOutput(m, &contours, &threshold);
      cv::vector<cv::Rect> good_rects;
      d.GetGoodBoundingRectsOfContours(contours, &good_rects);
      auto v = d.GetObjectsFromRects(good_rects, threshold, img.matrix());
      for(auto obj : v) {
        ShowImage(obj.image());
      }
    }
    return true;
  }
};
}  // namespace object_clustering
#endif  // OBJECT_CLUSTERING_OBJECT_DETECTOR_TEST_H_

