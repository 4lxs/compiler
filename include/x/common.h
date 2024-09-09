#pragma once

#include <memory>

namespace x {

template <typename T>
using Ptr = std::unique_ptr<T>;

template <typename T>
using Rc = std::shared_ptr<T>;

/// for std::visit
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

}  // namespace x
