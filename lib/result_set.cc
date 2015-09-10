//
// Created by itechbear on 9/10/15.
//

#include "result_set.h"

ResultSet::ResultSet(const size_t capacity)
    : capacity_(capacity), queue_() {
}

void ResultSet::Insert(const ResultItem &item) {
  if (!queue_.empty() && item < queue_.top()) {
    return;
  }
  queue_.push(item);
  if (queue_.size() > capacity_) {
    queue_.pop();
  }
}

std::priority_queue<ResultItem, std::vector<ResultItem>, std::greater<ResultItem>> &ResultSet::GetQueue() {
  return queue_;
}

const size_t ResultSet::GetCapacity() const {
  return capacity_;
}
