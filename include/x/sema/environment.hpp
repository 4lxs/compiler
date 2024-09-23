#pragma once

#include <vector>

#include "spdlog/spdlog.h"
#include "x/ast/decl.hpp"
#include "x/ast/fwd_decl.hpp"
#include "x/ast/type.hpp"
#include "x/pt/fwd_decl.hpp"

namespace x::sema {

class SymbolTable {
 public:
  struct Ident {
    std::string_view name;
    std::variant<not_null<ast::Decl *>, not_null<ast::Type *>> val;
  };

  [[nodiscard]] not_null<ast::Decl *> resolve_decl(
      not_null<pt::DeclRef *> var) const {
    Ident ident = resolve(var);
    if (auto *decl = std::get_if<not_null<ast::Decl *>>(&ident.val)) {
      return *decl;
    }
    throw std::runtime_error(fmt::format("expected decl {}", ident.name));
  }

  [[nodiscard]] not_null<ast::Type *> resolve_type(
      not_null<pt::DeclRef *> var) const {
    Ident ident = resolve(var);
    if (auto *type = std::get_if<not_null<ast::Type *>>(&ident.val)) {
      return *type;
    }
    throw std::runtime_error(fmt::format("expected type {}", ident.name));
  }

  void add_decl(not_null<ast::Decl *> decl) {
    spdlog::info("adding {}.", decl->name());
    _env.push_back(Ident{decl->name(), decl});
  }

  void add_type(not_null<ast::Type *> type) {
    spdlog::info("adding type {}.", type->name());
    _env.push_back(Ident{type->name(), type});
  }

  [[nodiscard]] not_null<ast::FnDecl *> get_struct_method(
      not_null<ast::StructTy *> type) const;

  using BlockRef = size_t;

  [[nodiscard]] BlockRef scope_begin() const { return _env.size(); }

  void scope_end(BlockRef ref) {
    _env.erase(_env.begin() + long(ref), _env.end());
  }

 private:
  [[nodiscard]] Ident resolve(not_null<pt::DeclRef *> var) const;

  std::vector<Ident> _env;
};
};  // namespace x::sema
