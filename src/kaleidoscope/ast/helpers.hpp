#pragma once

#include <ostream>

constexpr std::ostream& indent(std::ostream& ostream, size_t size) {
  return ostream << std::string(size, ' ');
}
