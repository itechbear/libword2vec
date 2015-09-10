//
// Created by itechbear on 9/10/15.
//

#ifndef LIB_RESULT_ITEM_H_
#define LIB_RESULT_ITEM_H_

#include <string>

class ResultItem {
 public:
  ResultItem(const float distance,
             const std::string &token);

  bool operator<(const ResultItem &other) const;

  bool operator>(const ResultItem &other) const;

  float GetDistance() const;

  const std::string &GetToken() const;

 private:
  float distance_;
  std::string token_;
};

#endif  // LIB_RESULT_ITEM_H_
