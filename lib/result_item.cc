//
// Created by itechbear on 9/10/15.
//

#include "result_item.h"

ResultItem::ResultItem(const float distance, const std::string &token)
    : distance_(distance), token_(token) {
}

bool ResultItem::operator<(const ResultItem &other) const {
  return distance_ < other.distance_;
}

bool ResultItem::operator>(const ResultItem &other) const {
  return distance_ > other.distance_;
}

float ResultItem::GetDistance() const {
  return distance_;
}

const std::string &ResultItem::GetToken() const {
  return token_;
}
