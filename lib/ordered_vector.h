//
// Created by itechbear on 9/6/15.
//

#ifndef LIB_ORDERED_VECTOR_H_
#define LIB_ORDERED_VECTOR_H_

#include <vector>
#include <map>

template <typename Element>
class OrderedVector {
 public:
  OrderedVector()
      : vector_(), positions_(), size_(0) {
  }

  ~OrderedVector() {
  }

  void Insert(const Element &element) {
    vector_.push_back(element);
    positions_[element] = vector_.size() - 1;
  }

  size_t GetSize() const {
    return vector_.size();
  }

  int64_t Find(const Element &element) const {
    typename std::map<Element, int>::const_iterator const_iterator
        = positions_.find(element);
    if (const_iterator == positions_.end()) {
      return -1;
    }

    return positions_.at(element);
  }

  const Element &Get(int index) const {
    return vector_.at(index);
  }

  void Shrink() {
    vector_.shrink_to_fit();
  }

 private:
  OrderedVector(const OrderedVector<Element> &other);
  OrderedVector<Element> &operator=(const OrderedVector<Element> &other);

  std::vector<Element> vector_;
  std::map<Element, int> positions_;
  size_t size_;
};

#endif  // LIB_ORDERED_VECTOR_H_
