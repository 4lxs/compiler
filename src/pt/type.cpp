#include "x/pt/type.hpp"

namespace x::pt {

TypeV* Type::validate() {
  if (_val != nullptr) {
    return _val.get();
  }
  _val = std::make_unique<TypeV>(_kind);

  return _val.get();
}

}  // namespace x::pt
