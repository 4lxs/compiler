#include "x/pt/path.hpp"

#include <fmt/format.h>

namespace x::pt {

Path::Path(std::vector<std::string> &&components, bool external)
    : _externalPath(external), _components{std::move(components)} {}

std::string format_as(Path const &path) {
  return fmt::format("{}{}", path._externalPath ? "::" : "",
                     fmt::join(path._components, "::"));
}

}  // namespace x::pt
