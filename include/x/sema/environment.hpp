#pragma once

#include <string_view>
#include <vector>

#include "x/ast/fwd_decl.hpp"
#include "x/pt/fwd_decl.hpp"

namespace x::sema {

class Environment {
 public:
  struct Ident {
    std::string_view name;
    ast::NamedDecl decl;
  };

  [[nodiscard]] ast::NamedDecl resolve(pt::DeclRef* var) const;

  void add(auto* decl) { _decls.emplace_back(Ident{decl->name(), decl}); }

  std::vector<Ident> _decls;
};

};  // namespace x::sema
