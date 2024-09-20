#pragma once

#include <string>
#include <vector>

#include "fwd_decl.hpp"
#include "x/common.hpp"

namespace x::pt {

struct Path : public AllowAlloc<Context, Path> {
 public:
  /// @param external ::a::b true, a::b false
  /// @param module {"a", "b"} in both cases
  explicit Path(std::vector<std::string> &&components, bool external = false);

  /// path external to this project. in x they
  /// are represented by leading :: e.g. ::a
  bool _externalPath;

  std::vector<std::string> _components;

  Path operator+(std::string suffix) const {
    std::vector components{_components};
    components.push_back(std::move(suffix));
    return Path{std::move(components), _externalPath};
  }

  constexpr std::strong_ordering operator<=>(Path const &other) const {
    if (_externalPath != other._externalPath) {
      return _externalPath ? std::strong_ordering::greater
                           : std::strong_ordering::less;
    }
    return _components <=> other._components;
  };
};

std::string format_as(Path const &path);

}  // namespace x::pt
