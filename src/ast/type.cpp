#include "x/ast/type.hpp"

namespace x::ast {

TypeV* Type::validate() {
  if (_val != nullptr) {
    return _val.get();
  }
  _val = std::make_unique<TypeV>(_kind);

  return _val.get();
}

}  // namespace x::ast
