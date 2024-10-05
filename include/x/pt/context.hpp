#pragma once

#include <llvm/Support/Allocator.h>

#include "x/common.hpp"
#include "x/pt/fwd_decl.hpp"
#include "x/pt/node.hpp"
#include "x/pt/sema/nameresolution.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::pt {

/// keeps data about the entire parse tree
/// allowing you to get stuff from other modules
/// even if those modules weren't processed yet.
///
/// while processing one module, we get to
/// use a::b::Type
/// you query context->getType(). the context will
/// give you a type. when later processing module a::b,
/// you call context->add(a::b, Type). the context will
/// be able to link those two together
class Context {
 public:
  static Ptr<Context> Create() {
    return std::unique_ptr<Context>(new Context());
  }

  void dump();

  void add_item(NodeId item) { _items.push_back(item); }

  [[nodiscard]] Node &get_node(NodeId nid) { return *_nodes.at(nid._id); }
  [[nodiscard]] Node const &get_node(NodeId nid) const {
    return *_nodes.at(nid._id);
  }

  void resolve_names();

  [[nodiscard]] sema::NameResolver const &name_resolver() const {
    return *_res;
  }

  template <typename T, typename... Args>
    requires(std::is_base_of_v<Node, T>)
  T &create(Args &&...args) {
    Ptr<T> ptr = make_unique<T>(std::forward<Args>(args)...);
    T &ref = *ptr;
    // auto &table = get_table<T>();
    // table.emplace_back(ptr);
    ref._id = uint32_t(_nodes.size());
    _nodes.push_back(std::move(ptr));
    return ref;
  }

  std::vector<Ptr<Node>> _nodes;

  std::vector<NodeId> _items;

  Ptr<sema::NameResolver> _res;

 private:
  Context();

  // mutable llvm::BumpPtrAllocator _allocator;

  template <typename T, typename... Args>
  Ptr<T> make_unique(Args &&...args) const {
    // constexpr auto align = 8;
    // auto *ptr = std::bit_cast<T *>(_allocator.Allocate(sizeof(T), align));
    // new (ptr) T(std::forward<Args>(args)...);

    return Ptr<T>(new T(std::forward<Args>(args)...));
  }

  //   template <typename T>
  //   using Table = std::vector<Ptr<T>>;
  //
  //   template <typename T>
  //   T &get_item() {}
  //
  //   template <typename T>
  //   Table<T> &get_table() = delete;
  //
  // #define TABLE(T, N)       \
//   template <>             \
//   Table<T> &get_table() { \
//     return N;             \
//   }                       \
//   Table<T> N;
  //
  // #undef TABLE
};

}  // namespace x::pt
