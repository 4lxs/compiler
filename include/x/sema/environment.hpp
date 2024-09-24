#pragma once

#include <vector>

#include "spdlog/spdlog.h"
#include "x/ast/decl.hpp"
#include "x/ast/fwd_decl.hpp"
#include "x/ast/type.hpp"
#include "x/pt/fwd_decl.hpp"

namespace x::sema {

struct TypeRef {
  int32_t index;
};

struct DeclRef {
  int32_t index;
};

class SymbolTable {
 public:
 private:
  struct Ident {
    std::string_view name;
    std::variant<DeclRef, TypeRef> val;
  };

 public:
  [[nodiscard]] not_null<ast::Decl *> resolve_decl(
      not_null<pt::DeclRef *> var) {
    Ident ident = resolve(var);
    if (auto *decl = std::get_if<DeclRef>(&ident.val)) {
      return get_decl(*decl);
    }
    throw std::runtime_error(fmt::format("expected decl {}", ident.name));
  }

  [[nodiscard]] not_null<ast::Type *> resolve_type(
      not_null<pt::DeclRef *> var) {
    Ident ident = resolve(var);
    if (auto *type = std::get_if<TypeRef>(&ident.val)) {
      return get_type(*type);
    }
    throw std::runtime_error(fmt::format("expected type {}", ident.name));
  }

  void add_decl(not_null<ast::Decl *> decl) {
    spdlog::info("adding {}.", decl->name());
    _env.push_back(Ident{decl->name(), DeclRef{int32_t(_decls.size())}});
    _decls.push_back(decl);
  }

  TypeRef add_type(not_null<ast::Type *> type) {
    spdlog::info("adding type {}.", type->name());
    TypeRef ref{int32_t(_types.size())};
    _env.push_back(Ident{type->name(), ref});
    _types.push_back(type);
    return ref;
  }

  TypeRef declare_type(std::string_view name) {
    spdlog::info("declaring type {}.", name);
    TypeRef ref{int32_t(_types.size())};
    _env.push_back(Ident{name, ref});
    _types.push_back(nullptr);
    return ref;
  }

  void define_type(TypeRef ref, not_null<ast::Type *> def) {
    spdlog::info("defining type {}.", def->name());
    ast::Type *&type = get_type(ref);
    assert(type == nullptr);
    type = def;
  }

  using BlockRef = size_t;

  [[nodiscard]] BlockRef scope_begin() const { return _env.size(); }

  void scope_end(BlockRef ref) {
    _env.erase(_env.begin() + long(ref), _env.end());
  }

 private:
  [[nodiscard]] ast::Decl *&get_decl(DeclRef ref) {
    return _decls.at(size_t(ref.index));
  }

  [[nodiscard]] ast::Type *&get_type(TypeRef ref) {
    return _types.at(size_t(ref.index));
  }

  [[nodiscard]] Ident resolve(not_null<pt::DeclRef *> var) const;

  std::vector<Ident> _env;
  std::vector<ast::Decl *> _decls;
  std::vector<ast::Type *> _types;
};
};  // namespace x::sema
