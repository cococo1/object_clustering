// Copyright Max Chetrusca, Oct 28 2014
// image_test.h
// Object Clustering
// A test class for Image class.
#ifndef OBJECT_CLUSTERING_IMAGE_TEST_H_
#define OBJECT_CLUSTERING_IMAGE_TEST_H_

#include <cassert>
#include "image.h"
#include "gui_functions.h"

namespace object_clustering {
class ImageTest {
 public:
 static bool TestImage() {
   ImageTest image_test;
   return image_test.TestCreation() &&
          image_test.TestSettersAndGetters();
 }
  bool TestCreation() {
    //Image i1; // this should give a compile error.
    Image i("images/1-2.png");
    ShowImage(i);

    Image i_copy(i);
    ShowImage(i_copy);

    Image i_copy2 = i;
    ShowImage(i_copy2);

    i = i_copy;
    ShowImage(i);

    //Image i2("asdad");  // this should give a runtime error
    
    return true;
  }
  bool TestSettersAndGetters() {
    Image i("images/1-2.png");
    cv::Rect r(0,0,1280,960);
    assert(r == i.bounding_rect());
    cv::Mat m(cv::imread("images/1-2.png", CV_LOAD_IMAGE_COLOR)); 
    cv::Point2d p(640, 480);
    assert(i.GetCenter() == p);

    Image i2(m);
    i2.set_bounding_rect(r);
    assert(i2.GetCenter() == p);
    return true;
  }
};
}  // namespace object_clustering
#endif  // OBJECT_CLUSTERING_IMAGE_TEST_H_

