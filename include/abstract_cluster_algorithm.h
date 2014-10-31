// Copyright Max Chetrusca, Oct 27 2014
// abstract_cluster_algorithm.h
// Object Clustering
// Declares an abstract class which defines an interface for any clustering
// algorithm in this project.

#ifndef OBJECT_CLUSTERING_ABSTRACT_CLUSTER_ALGORITHM_H_
#define OBJECT_CLUSTERING_ABSTRACT_CLUSTER_ALGORITHM_H_

#include <string>
#include <vector>

#include "object.h"

// This is an abstract class which defines a common behaviour for any
// clustering algorithm
namespace object_clustering {
class AbstractClusterAlgorithm {
 public:
  AbstractClusterAlgorithm() = default;

  AbstractClusterAlgorithm(const AbstractClusterAlgorithm &algorithm) = default;

  AbstractClusterAlgorithm& operator=(const AbstractClusterAlgorithm& algorithm)
    = default;

  virtual ~AbstractClusterAlgorithm() = default;
  // any clustering algorithm should be able to label a vector of objects;
  // basically, this method should set_group() of the objects
  virtual int AssignGroupsToObjects(std::vector<Object> *objects) const =
  0;
  // an algorithm is identified by its name:
  std::string get_name() const { return name_; }

  void set_name(const std::string &name) { name_ = name; }
 private:
  std::string name_ = "unknown";
};
}  // namespace object_clustering
#endif  // OBJECT_CLUSTERING_ABSTRACT_CLUSTER_ALGORITHM_H_

