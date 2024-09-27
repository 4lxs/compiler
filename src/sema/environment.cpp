#include "x/sema/environment.hpp"

#include <llvm/Support/Casting.h>

#include <cassert>
#include <ranges>

#include "spdlog/spdlog.h"
#include "x/pt/expr.hpp"

namespace x::sema {

//===
//= LookupResult
//===

ast::Decl *LookupResult::get_single() const {
  assert(_kind == ResultType::Found && _decls.size() == 1);

  return _decls.front();
}

//===
//= Namespace
//===

Ptr<LookupResult> Namespace::lookup(std::span<std::string> path) const {
  assert(!path.empty());
  auto itr = _items.find(path.front());
  if (itr == _items.end()) {
    return std::make_unique<LookupResult>();
  }

  if (path.size() == 1) {
    return std::make_unique<LookupResult>(
        LookupResult::ResultType::Found, std::vector<ast::Decl *>{itr->second});
  }

  ast::Decl *decl = itr->second;

  if (decl->get_kind() != ast::Decl::DeclKind::Struct) {
    spdlog::error("not implemented for {}", fmt::underlying(decl->get_kind()));
    throw std::runtime_error("decl doesn't have children");
  }

  auto *structTy = llvm::cast<ast::StructTy>(decl);
}

//===
//= SymbolTable
//===

std::string_view mangle_name(ast::Decl *decl) {
  switch (decl->get_kind()) {
    case ast::Decl::DeclKind::Fn:
    case ast::Decl::DeclKind::Var:
    case ast::Decl::DeclKind::Field:
    case ast::Decl::DeclKind::Type:
    case ast::Decl::DeclKind::Literal:
    case ast::Decl::DeclKind::Struct:
    case ast::Decl::DeclKind::Union:
    case ast::Decl::DeclKind::TypeEnd:
      return decl->name();
    case ast::Decl::DeclKind::ValueBegin:
    case ast::Decl::DeclKind::ValueEnd:
      throw std::runtime_error("invalid declkind");
  }
}

Ptr<LookupResult> SymbolTable::lookup(not_null<pt::DeclRef *> var) const {
  spdlog::info("resolving variable {}", format_as(var->_var));
  auto &names = var->_var._components;
  assert(!names.empty());

  LookupResult res = lookup(names.front());

  auto join = [](ast::Decl *decl, std::string_view name) {
    switch (decl->get_kind()) {
      case ast::Decl::DeclKind::Struct:
        return fmt::format("{}%s{}", decl->name(), name);
      case ast::Decl::DeclKind::Type:
      case ast::Decl::DeclKind::Union:
      case ast::Decl::DeclKind::Fn:
      case ast::Decl::DeclKind::Var:
      case ast::Decl::DeclKind::Field:
      case ast::Decl::DeclKind::Literal:
      case ast::Decl::DeclKind::ValueBegin:
      case ast::Decl::DeclKind::ValueEnd:
      case ast::Decl::DeclKind::TypeEnd:
        throw std::runtime_error(
            fmt::format("unexpected declkind for {}:{}", decl->name(),
                        fmt::underlying(decl->get_kind())));
    }
  };

  for (auto itr = names.begin() + 1; itr != names.end(); ++itr) {
    if (res._kind != LookupResult::ResultType::Found) {
      break;
    }
    res = lookup(join(res.get_single(), *itr));
  }
  return std::make_unique<LookupResult>(std::move(res));
}

LookupResult SymbolTable::lookup(std::string_view name) const {
  std::vector<ast::Decl *> found;

  for (ast::Decl *item : _items | std::views::reverse) {
    if (item == nullptr) {
      if (!found.empty()) {
        // end of scope in which we found a match
        break;
      }
      continue;
    }
    if (item->name() == name) {
      found.push_back(item);
    }
  }

  if (found.empty()) {
    return {};
  }

  if (found.size() == 1) {
    return LookupResult(LookupResult::ResultType::Found, std::move(found));
  }

  throw std::runtime_error("unimplemented overloading");
}

}  // namespace x::sema
