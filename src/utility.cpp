#include "utility.h"

bool IsNullTerminated(std::string_view sv) {
  return !sv.empty() && sv.back() == '\0';
}