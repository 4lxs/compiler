#pragma once

#include <map>

#include "x/ast/context.hpp"
#include "x/ast/fwd_decl.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/context.hpp"
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
  explicit Sema(pt::Context* ctx);

  Ptr<ast::Context> finish();

 private:
  ast::Fn* check(pt::Fn* func);
  ast::Type* check(pt::Type* type);
  ast::Return* check(Ptr<pt::RetStmt>& stmt);
  not_null<ast::Expr*> check(pt::Expr& expr);

  ast::Block* check(pt::Block& block);
  ast::StructLiteral* check(pt::StructExpr& expr);

  void add(pt::Module const& module);

  Ptr<ast::Context> _ast = std::make_unique<ast::Context>();
  pt::Context* _pt;

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
    explicit Mappings(std::map<pt::Type*, ast::Type*> typeMap)
        : _typeMap(std::move(typeMap)) {
      for (auto& [_, ast] : _typeMap) {
        spdlog::info("initializing {}", fmt::ptr(ast));
        ast = initialized(ast);
      }
    }

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
      bool isInitialized = is_init(itr->second);
      if (init) {
        itr->second = initialized(itr->second);
      }
      return Return<AstType>{
          .astValue = get_ptr(itr->second),
          .wasInitialized = isInitialized,
      };
    }

    template <typename PtType, typename AstType>
    void set(std::map<PtType*, AstType*>& map, PtType* pt, AstType* ast,
             bool init) {
      map.insert({pt, init ? initialized(ast) : ast});
    }

    template <typename T>
    constexpr T* initialized(T* ptr) {
      return reinterpret_cast<T*>(uintptr_t(ptr) | uintptr_t(1));
    }

    template <typename T>
    constexpr T* get_ptr(T* ptr) {
      return reinterpret_cast<T*>(uintptr_t(ptr) & ~uintptr_t(1));
    }

    bool is_init(void* ptr) { return (uintptr_t(ptr) & uintptr_t(1)) != 0; }
  };
  Mappings _maps{std::map<pt::Type*, ast::Type*>{
      {_pt->_strTy.get(), _ast->_strTy},
      {_pt->_numTy.get(), _ast->_int32Ty},
      {_pt->_boolTy.get(), _ast->_boolTy},
  }};
};

}  // namespace x::sema
