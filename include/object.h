// Copyright Max Chetrusca, Oct 23 2014
// object.h
// Object clustering
// Declaration of a class which represents an Object.

#ifndef OBJECT_CLUSTERING_OBJECT_H_
#define OBJECT_CLUSTERING_OBJECT_H_

#include <cassert>

#include "opencv2/opencv.hpp"

#include "image.h"

namespace object_clustering {

// Class which identifies an object by its image and group
const int kNoGroup = -1;  // signifies that the object does not have a group
// Usage:
// object_clustering::Image image = ...;
// object_clustering::Object o(image);
// o.set_group(2); //belongs to group 2.
class Object {
 public:
  // an object is represented by an image;
  Object() = delete;

  explicit Object(const Image &image):
    image_(image),
    group_(kNoGroup),
    grouped_(false) {}

  Object(const Object &object) = default;

  Object& operator=(const Object &object) = default;

  virtual ~Object() = default;

  int group() const { return group_; }

  bool grouped() const { return grouped_; }

  Image image() const { return image_; }
  // parameter group should have values >= -1;
  // A group is negative only when set to -1 which signifies that this object
  // does not have a group, otherwise it should be a positive number:
  void set_group(int group) {
    assert(group >= -1);
    group_ = group;
    grouped_ = true;
  }

 private:
    Image image_;  // initialized in constructor
    int group_ = kNoGroup;  // initially the object has no group
    bool grouped_ = false;  // the object is not grouped.
};
}  // namespace object_clustering
#endif  // OBJECT_CLUSTERING_OBJECT_H_

