#ifndef ARGOS_NETWORKAPI_UTILITIES_H
#define ARGOS_NETWORKAPI_UTILITIES_H

#include <algorithm>

template <typename T>
static inline void EraseFromVector(std::vector<T>& deque, T const& value) {
  deque.erase(std::remove(begin(deque), end(deque), value), end(deque));
};

static inline std::vector<std::string> SplitString(
  const std::string& str, const std::string& delims = " ") {
  std::vector<std::string> output;
  auto first = std::cbegin(str);

  while (first != std::cend(str)) {
    const auto second = std::find_first_of(
      first, std::cend(str), std::cbegin(delims), std::cend(delims));

    if (first != second) output.emplace_back(first, second);

    if (second == std::cend(str)) break;

    first = std::next(second);
  }

  return output;
}

// uses std::find_first_of rather than string_view::find_first_of
static inline std::vector<std::string_view> SplitSV(
  std::string_view strv, std::string_view delims = " ") {
  std::vector<std::string_view> output;
  // output.reserve(strv.length() / 4);
  auto first = strv.begin();

  while (first != strv.end()) {
    const auto second = std::find_first_of(
      first, std::cend(strv), std::cbegin(delims), std::cend(delims));
    // std::cout << first << ", " << second << '\n';
    if (first != second) {
      output.emplace_back(strv.substr(
        std::distance(strv.begin(), first), std::distance(first, second)));
    }

    if (second == strv.end()) break;

    first = std::next(second);
  }

  return output;
}
#endif