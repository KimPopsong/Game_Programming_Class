#include "stdafx.h"
#include "NameScoreStack.h"

NameScoreStack::~NameScoreStack() {}

NameScoreStack::NameScoreStack(size_t maxSize) { _maxSize = maxSize; }

void NameScoreStack::emplace(std::pair<const TCHAR*, int>&& element) {
  if (!full()) {
    nameScoreStack.emplace(element);
  }
}

std::pair<const TCHAR*, int> NameScoreStack::top() const {
  return nameScoreStack.top();
}

void NameScoreStack::pop() { nameScoreStack.pop(); }

size_t NameScoreStack::maxSize() const { return _maxSize; }

bool NameScoreStack::full() const { return nameScoreStack.size() >= _maxSize; }

bool NameScoreStack::empty() const { return nameScoreStack.empty(); }

size_t NameScoreStack::size() const { return nameScoreStack.size(); }
