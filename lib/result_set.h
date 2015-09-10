//
// Created by itechbear on 9/10/15.
//

#ifndef LIB_RESULT_SET_H_
#define LIB_RESULT_SET_H_

#include <queue>

#include "result_item.h"

class ResultSet {
 public:
  explicit ResultSet(const size_t capacity);

  const size_t GetCapacity() const;

  void Insert(const ResultItem &item);

  std::priority_queue<ResultItem, std::vector<ResultItem>, std::greater<ResultItem>> &GetQueue();

 private:
  const size_t capacity_;
  std::priority_queue<ResultItem, std::vector<ResultItem>, std::greater<ResultItem>> queue_;
};

#endif  // LIB_RESULT_SET_H_
