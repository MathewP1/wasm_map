#ifndef C_STRING_VIEW_H
#define C_STRING_VIEW_H

#include <string>
#include <string_view>

// Class that modifies std::string_view to be null terminated
class CStringView : public std::string_view {
 public:
  constexpr CStringView() = default;
  constexpr CStringView(const char* str)
      : std::string_view(str, FindLength(str)) {}
  constexpr CStringView(const char* str, std::size_t length)
      : std::string_view(str, length) {}
  constexpr CStringView(const std::string& str)
      : std::string_view(str.c_str()) {}

  // Create from string literal without iterating through it
  template <std::size_t N>
  constexpr CStringView(const char (&str)[N]) : std::string_view(str, N) {}

  // Prevent implicit conversion from std::string_view because it might not be
  // null terminated
  CStringView(std::string_view) = delete;

 private:
  static constexpr std::size_t FindLength(const char* str) {
    std::size_t length = 0;
    while (str[length] != '\0') {
      ++length;
    }
    return length;
  }
};

#endif