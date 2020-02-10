#ifndef NETWORK_UTILITIES_H
#define NETWORK_UTILITIES_H

template <typename T>
static inline void EraseFromVector(std::vector<T>& deque, T const& value) {
  deque.erase(std::remove(begin(deque), end(deque), value), end(deque));
};

#endif