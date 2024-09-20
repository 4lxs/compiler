#pragma once

#include <span>
#include <vector>

#include "spdlog/spdlog.h"
#include "x/ast/fwd_decl.hpp"
#include "x/ast/stmt.hpp"
#include "x/pt/fwd_decl.hpp"

namespace x::sema {

class SymbolTable {
 public:
  struct Ident {
    std::string_view name;
    not_null<ast::Decl*> decl;
  };

  [[nodiscard]] not_null<ast::Decl*> resolve(not_null<pt::DeclRef*> var) const;

  std::span<Ident> global_decls() { return _decls; }

  void add(std::string_view name, not_null<ast::Decl*> decl) {
    spdlog::info("adding {}.", name);
    _decls.push_back(Ident{name, decl});
  }

  void add(not_null<ast::Decl*> decl) { add(decl->name(), decl); }

  using BlockRef = size_t;

  [[nodiscard]] BlockRef scope_begin() const { return _decls.size(); }

  void scope_end(BlockRef ref) {
    _decls.erase(_decls.begin() + long(ref), _decls.end());
  }

  std::vector<Ident> _decls;
};
};  // namespace x::sema
