#pragma once

#include <string_view>

#include "fwd_decl.hpp"

namespace x::ast {

class Label {
 public:
  explicit Label(std::string_view name = "") : _name(name) {}

  [[nodiscard]] Block& block() const {
    assert(_block != nullptr);
    return *_block;
  }

  std::string_view _name;

  // private: TODO:
  friend class Block;
  /// owned by the block. when the block is destroyed, this field is set to null
  Block* _block{};
};

}  // namespace x::ast
