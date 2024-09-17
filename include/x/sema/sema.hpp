#pragma once

#include <map>

#include "x/ast/context.hpp"
#include "x/ast/fwd_decl.hpp"
#include "x/ast/stmt.hpp"
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
  ast::Return* check(Ptr<pt::RetStmt>& stmt);
  ast::Expr* check(pt::Expr& expr, pt::Type* type = nullptr);

  ast::Block* check(pt::Block& block);
  ast::StructLiteral* check(pt::StructExpr& expr);

  void add(pt::Module const& module);

  Ptr<ast::Context> _ast = std::make_unique<ast::Context>();

  /// maps parse tree types to ast types
  /// note that ast types are allocated in ast::Context and always guaranteed to
  /// be aligned to 8 bytes. this allows us to store info in lower 3 bits of the
  /// pointer
  ///
  /// last bit is 1 if the type is initialized (that is Create() has been called
  /// on it)
  class Mappings {
    std::map<pt::Type*, ast::Type*> _typeMap;
    std::map<pt::Fn*, ast::Fn*> _fnMap;

   public:
    template <typename T>
    struct Return {
      T* astValue;

      /// true if astValue was initialized before call to get
      bool wasInitialized;
    };

    Return<ast::Fn> get(pt::Fn* type, bool initialize = false) {
      return get(_fnMap, type, initialize);
    };

    Return<ast::Type> get(pt::Type* type, bool initialize = false) {
      return get(_typeMap, type, initialize);
    };

    void insert(pt::Type* pt, ast::Type* ast, bool initialize = false) {
      set(_typeMap, pt, ast, initialize);
    }

    void insert(pt::Fn* pt, ast::Fn* ast, bool initialize = false) {
      set(_fnMap, pt, ast, initialize);
    }

   private:
    template <typename PtType, typename AstType>
    Return<AstType> get(std::map<PtType*, AstType*>& map, PtType* type,
                        bool init) {
      auto itr = map.find(type);
      assert(itr != map.end());
      bool initialized = (uintptr_t(itr->second) & uintptr_t(1)) != 0;
      if (init) {
        itr->second =
            reinterpret_cast<AstType*>(uintptr_t(itr->second) | uintptr_t(1));
      }
      return Return<AstType>{
          .astValue = reinterpret_cast<AstType*>(uintptr_t(itr->second) &
                                                 ~uintptr_t(1)),
          .wasInitialized = initialized,
      };
    }

    template <typename PtType, typename AstType>
    void set(std::map<PtType*, AstType*>& map, PtType* pt, AstType* ast,
             bool init) {
      map.insert(
          {pt, init ? reinterpret_cast<AstType*>(uintptr_t(ast) | uintptr_t(1))
                    : ast});
    }
  } _maps;

  ast::Type* _voidType = ast::Type::Create(*_ast, ast::Type::Kind::Void);
  ast::Fn* _builtinAddInt = ast::Fn::Create(*_ast, );
};

}  // namespace x::sema
