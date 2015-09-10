//
// Created by itechbear on 9/10/15.
//

#ifndef RESULT_SET_H_
#define RESULT_SET_H_

#include <queue>

#include "result_item.h"

class ResultSet {
 public:
  explicit ResultSet(const size_t capacity);

  void Insert(const ResultItem &item);

  std::priority_queue<ResultItem, std::vector<ResultItem>, std::greater<ResultItem>> &GetQueue();

 private:
  const size_t capacity_;
  std::priority_queue<ResultItem, std::vector<ResultItem>, std::greater<ResultItem>> queue_;
};

#endif  // RESULT_SET_H_
