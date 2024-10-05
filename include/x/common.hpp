#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <gsl/gsl>
#include <memory>

#include "spdlog/spdlog.h"

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

template <typename Alloc, typename Item>
concept CanAlloc = requires(Alloc all, Item item) {
  { all.template allocate<Item>() } -> std::convertible_to<Item*>;
};

template <typename Allocator, typename Self>
  requires CanAlloc<Allocator, Self>
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

template <typename... Args>
[[noreturn]] inline void xerr(fmt::format_string<Args...> fmt, Args&&... args) {
  spdlog::error(fmt, std::forward<Args>(args)...);
  std::terminate();
}

}  // namespace x
