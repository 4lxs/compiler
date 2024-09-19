#include "x/pt/stmt.hpp"

#include <string>

#include "x/pt/module.hpp"

namespace x::pt {

std::string const& Fn::name() const { return _stub->name(); }

}  // namespace x::pt
