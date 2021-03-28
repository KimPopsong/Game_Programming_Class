#pragma once

#include <stack>
#include <tchar.h>

class NameScoreStack {
  std::stack<std::pair<const TCHAR*, int>> nameScoreStack,
      nameScoreStackHelperStack;
  size_t _maxSize;

 public:

  ~NameScoreStack();

  NameScoreStack(size_t maxSize);

  void emplace(std::pair<const TCHAR*, int>&& element);
  std::pair<const TCHAR*, int> top() const;
  void pop();
  bool full() const;
  bool empty() const;
  size_t size() const;
  size_t maxSize() const;

  template <typename Consumer>
  void forEachFromBottom(Consumer consumer) {
    while (!nameScoreStack.empty()) {
      nameScoreStackHelperStack.push(nameScoreStack.top());
      nameScoreStack.pop();
    }
    while (!nameScoreStackHelperStack.empty()) {
      auto& element = nameScoreStackHelperStack.top();
      nameScoreStackHelperStack.pop();
      consumer(element);
      nameScoreStack.push(element);
    }
  }
};
