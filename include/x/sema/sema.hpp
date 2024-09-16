#pragma once

#include <map>
#include <vector>

#include "x/ast/ast.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/pt.hpp"
#include "x/pt/type.hpp"

namespace x::sema {

/// checks for inconsistencies. undefined stubs, multiple definitions, ...
/// it also evaluates items to their definition and links them with other stubs.
///
/// note that calling this function invalidates the context and all pointers
/// obtained from it except any pt::Path*, which is guaranteed to be alive as
/// long as Sema is alive and can be used to remove/update a node
class Sema {
 public:
  /// @param ctx the initial parse tree
  explicit Sema(pt::Context const& ctx);

  Ptr<ast::Context> finish();

 private:
  ast::Fn* check(pt::Fn* func);
  ast::Type* check(pt::Type* type);
  Ptr<ast::Ret> check(Ptr<pt::RetStmt>& stmt);
  ast::Expr check(pt::Expr& expr, pt::Type* type = nullptr);

  Ptr<ast::BlockE> check(pt::Block& block);
  Ptr<ast::StructE> check(pt::StructExpr& expr);

  void add(pt::Module const& module);

  std::vector<Ptr<ast::Module>> _modules;
  Ptr<ast::Type> _voidType = std::make_unique<ast::Type>(ast::Type::Kind::Void);

  std::map<pt::Type*, ast::Type*> _typeMap;
  std::map<pt::Fn*, ast::Fn*> _fnMap;
};

}  // namespace x::sema
