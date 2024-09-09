#pragma once

#include <memory>

namespace x {

template <typename T>
using Ptr = std::unique_ptr<T>;

template <typename T>
using Rc = std::shared_ptr<T>;

}  // namespace x
