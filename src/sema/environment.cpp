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

ast::DeclRef LookupResult::get_single() const {
  assert(_kind == ResultType::Found && _decls.size() == 1);

  return _decls.front()->definition();
}

//===
//= Namespace
//===

// Ptr<LookupResult> Namespace::lookup(std::span<std::string> path) const {
//   assert(!path.empty());
//   auto itr = _items.find(path.front());
//   if (itr == _items.end()) {
//     return std::make_unique<LookupResult>();
//   }
//
//   if (path.size() == 1) {
//     return std::make_unique<LookupResult>(
//         LookupResult::ResultType::Found, std::vector<ast::Decl
//         *>{itr->second});
//   }
//
//   ast::Decl *decl = itr->second;
//
//   if (decl->get_kind() != ast::Decl::DeclKind::Struct) {
//     spdlog::error("not implemented for {}",
//     fmt::underlying(decl->get_kind())); throw std::runtime_error("decl
//     doesn't have children");
//   }
//
//   auto *structTy = llvm::cast<ast::StructTy>(decl);
// }

//===
//= SymbolTable
//===

// std::string_view SymbolTable::mangle_join(ast::Decl *decl,
//                                           std::string_view name) const {
//   char joinCh{};
//   switch (decl->get_kind()) {
//     case ast::Decl::DeclKind::Struct:
//       joinCh = 's';
//       break;
//     case ast::Decl::DeclKind::Const:
//     case ast::Decl::DeclKind::Var:
//     case ast::Decl::DeclKind::Field: {
//       auto *var = llvm::cast<ast::ValueDecl>(decl);
//       return mangle_join(var->type(), name);
//     }
//     case ast::Decl::DeclKind::Fn:
//     case ast::Decl::DeclKind::Type:
//     case ast::Decl::DeclKind::Literal:
//     case ast::Decl::DeclKind::Union:
//     case ast::Decl::DeclKind::TypeEnd:
//     case ast::Decl::DeclKind::ValueBegin:
//     case ast::Decl::DeclKind::ValueEnd:
//       xerr("unexpected decl to join {}", fmt::underlying(decl->get_kind()));
//   }
//
//   _mangledNames.push_back(fmt::format("{}%{}{}", decl->name(), joinCh,
//   name)); spdlog::info("mangled name: {}", _mangledNames.back()); return
//   _mangledNames.back();
// }
//
// Ptr<LookupResult> SymbolTable::lookup(not_null<pt::DeclUse *> var) const {
//   spdlog::info("resolving variable {}", format_as(var->_var));
//   auto &names = var->_var._components;
//   assert(!names.empty());
//
//   LookupResult res = lookup(names.front());
//
//   for (auto itr = names.begin() + 1; itr != names.end(); ++itr) {
//     if (res._kind != LookupResult::ResultType::Found) {
//       break;
//     }
//     res = lookup(mangle_join(res.get_single(), *itr));
//   }
//   return std::make_unique<LookupResult>(std::move(res));
// }
//
// LookupResult SymbolTable::lookup(std::string_view name) const {
//   std::vector<ast::Decl *> found;
//
//   for (ast::Decl *item : _items | std::views::reverse) {
//     if (item == nullptr) {
//       if (!found.empty()) {
//         // end of scope in which we found a match
//         break;
//       }
//       continue;
//     }
//     if (item->name() == name) {
//       found.push_back(item);
//     }
//   }
//
//   if (found.empty()) {
//     spdlog::warn("not found {} in symbol table", name);
//     return {};
//   }
//
//   if (found.size() == 1) {
//     return LookupResult(LookupResult::ResultType::Found, std::move(found));
//   }
//
//   for (ast::Decl *decl : found) {
//     spdlog::info("found multiple decls with name {}: {}", decl->name(),
//                  fmt::underlying(decl->get_kind()));
//   }
//
//   xerr("unimplemented overloading");
// }

}  // namespace x::sema
