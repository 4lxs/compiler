#pragma once

#include <gsl/gsl>
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

template <typename Allocator, typename Self>
class AllowAlloc {
 public:
  static Self* Allocate(Allocator& allocator) {
    return allocator.template allocate<Self>();
  }

  template <typename... Args>
  static Self* Create(Allocator& allocator, Args&&... args) {
    Self* ret = allocator.template allocate<Self>();
    new (ret) Self(std::forward<Args>(args)...);
    return ret;
  }

  template <typename... Args>
  void Create(Args&&... args) {
    new (this) Self(std::forward<Args>(args)...);
  }
};

using gsl::not_null;

}  // namespace x
