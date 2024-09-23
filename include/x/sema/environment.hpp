#pragma once

#include <map>
#include <span>
#include <vector>

#include "spdlog/spdlog.h"
#include "x/ast/decl.hpp"
#include "x/ast/fwd_decl.hpp"
#include "x/pt/fwd_decl.hpp"

namespace x::sema {

class SymbolTable {
 public:
  struct Ident {
    std::string_view name;
    not_null<ast::Decl*> decl;
  };

  [[nodiscard]] not_null<ast::Decl*> resolve(not_null<pt::DeclRef*> var) const;

  [[nodiscard]] not_null<ast::FnDecl*> resolve_function(
      not_null<pt::DeclRef*> var, ast::StructLiteral* args) const;

  std::span<Ident> global_decls() { return _decls; }

  void add_decl(not_null<ast::Decl*> decl) {
    spdlog::info("adding {}.", decl->name());
    _decls.push_back(Ident{decl->name(), decl});
  }

  void add_struct_method(not_null<ast::StructTy*> type,
                         not_null<ast::FnDecl*> method);

  [[nodiscard]] not_null<ast::FnDecl*> get_struct_method(
      not_null<ast::StructTy*> type) const;

  using BlockRef = size_t;

  [[nodiscard]] BlockRef scope_begin() const { return _decls.size(); }

  void scope_end(BlockRef ref) {
    _decls.erase(_decls.begin() + long(ref), _decls.end());
  }

  std::vector<Ident> _decls;

  std::map<ast::StructTy*, std::vector<not_null<ast::FnDecl*>>> _methods;
};
};  // namespace x::sema
