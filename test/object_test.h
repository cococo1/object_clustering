// Copyright Max Chetrusca, Oct 28 2014
// object_test.h
// Object clustering
// A test class for Object class.
#ifndef OBJECT_CLUSTERING_OBJECT_TEST_H_
#define OBJECT_CLUSTERING_OBJECT_TEST_H_

#include <cassert>

#include "gui_functions.h"
#include "object.h"

namespace object_clustering {
class ObjectTest {
 public:
  static bool TestObject() {
    ObjectTest object_test;
    return object_test.TestCreation() &&
           object_test.TestSettersAndGetters();
  }
  bool TestCreation() {
    //Object o; // should not compile;
    Image img("images/3-1.png");
    Object obj(img);
    ShowImage(obj.image());
    Object obj2(obj);
    ShowImage(obj2.image());
    Object obj3(Image("images/3-2.png"));
    obj = obj3;
    ShowImage(obj.image());

    assert(obj.group() == -1);
    assert(obj.grouped() == false);

    return true;
  }
  bool TestSettersAndGetters() {
    Object obj(Image("images/4-1.png"));
    //obj.set_group(-2); // should give a runtime error
    obj.set_group(-1);
    obj.set_group(0);
    obj.set_group(1);
    assert(obj.grouped());
    assert(obj.group() == 1);
    return true;
  }
};
}  // namespace object_clustering
#endif  // OBJECT_CLUSTERING_OBJECT_TEST_H_

