#ifndef UTILITY_H
#define UTILITY_H

#include <string_view>

template <std::size_t N>
constexpr std::string_view NullTerminate(const char (&str_literal)[N]) {
  return {str_literal, N};
}

bool IsNullTerminated(std::string_view sv);

#endif