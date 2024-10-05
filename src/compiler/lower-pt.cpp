#include "lower-pt.hpp"

#include <llvm/ADT/APInt.h>
#include <llvm/Support/Casting.h>

#include <map>
#include <memory>

#include "x/ast/decl.hpp"
#include "x/ast/expr.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/pt/block.hpp"
#include "x/pt/decl.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/node.hpp"
#include "x/pt/sema/nameresolution.hpp"
#include "x/pt/stmt.hpp"

namespace x {

struct Data {
 public:
  explicit Data(std::unique_ptr<pt::Context> pt) : _pt(std::move(pt)) {}

  std::unique_ptr<ast::Context> lower() {
    for (pt::NodeId nodeid : _pt->_items) {
      pt::Node& node = _pt->get_node(nodeid);
      lower_decl(node);
    }

    _ast->_pt = std::move(_pt);
    return std::move(_ast);
  }

  Rc<ast::Decl> lower_decl(pt::Node& node) {
    Rc<ast::Decl>& decl = _decls[node.id()];
    if (decl != nullptr) {
      return decl;
    }

    switch (node.kind()) {
      case pt::Node::Kind::FnDecl: {
        Rc<ast::FnDecl> astdecl = lower_fn(llvm::cast<pt::FnDecl>(node));
        _ast->_functions.push_back(astdecl);
        return decl = std::move(astdecl);
      }
      case pt::Node::Kind::MethodDecl: {
        auto& method = llvm::cast<pt::MethodDecl>(node);
        return decl = lower_fn(method, &method);
      }
      case pt::Node::Kind::Primitive: {
        Rc<ast::LiteralTy> astdecl =
            lower_primitive(llvm::cast<pt::Primitive>(node));
        _ast->_types.push_back(astdecl);
        return decl = std::move(astdecl);
      }
      case pt::Node::Kind::VarDecl:
        return decl = lower_var(llvm::cast<pt::VarDecl>(node));
      default:
        xerr("unable to lower decl {}", fmt::underlying(node.kind()));
    }
  }

  Ptr<ast::VarDecl> lower_var(pt::VarDecl& node) {
    return std::make_unique<ast::VarDecl>(node.name(*_pt),
                                          get_type(node._type));
  }

  Rc<ast::LiteralTy> lower_primitive(pt::Primitive& node) {
    return _ast->_int32Ty = std::make_unique<ast::LiteralTy>(
               ast::LiteralTy::Kind::I32, node.name(*_pt));
  }

  Ptr<ast::FnDecl> lower_fn(pt::FnDecl& node,
                            pt::MethodDecl* method = nullptr) {
    bool hasSelf = method != nullptr && !method->_isStatic;

    std::vector<ast::FnDecl::Param> params;
    params.reserve(node._params.size() + (hasSelf ? 1 : 0));

    // if (method != nullptr) {
    //   Rc<ast::Decl> decl = declare_item(_pt->get_node(method->_recv));
    //   auto& recvtyp = llvm::cast<ast::Type>(*decl);
    //
    //   if (hasSelf) {
    //     params.push_back(ast::FnDecl::Param(pt->name(), recvtyp));
    //   }
    //
    //   pt->_mangledName = fmt::format("{}%s{}", recvtyp->name(), pt->name());
    // } else {
    //   pt->_mangledName = pt->name();
    // }
    //
    for (pt::NodeId paramnodeid : node._params) {
      pt::Node& paramnode = _pt->get_node(paramnodeid);
      if (paramnode.kind() != pt::Node::Kind::ParamDecl) {
        xerr("expected ParamDecl, got {}", fmt::underlying(paramnode.kind()));
      }

      auto& paramdecl = llvm::cast<pt::ParamDecl>(paramnode);

      Rc<ast::Type> restype = get_type(paramdecl.type);

      params.push_back(ast::FnDecl::Param(restype));
    }

    Rc<ast::Type> rettype = get_type(node._retTy);

    Ptr<ast::Block> body = lower_block(node._body);

    return std::make_unique<ast::FnDecl>(node.name(*_pt), std::move(params),
                                         rettype, std::move(body),
                                         std::move(body->_localvars));
  }

  Rc<ast::Type> get_type(pt::NodeId nodeid) {
    pt::Node& node = _pt->get_node(nodeid);
    assert(node.kind() == pt::Node::Kind::DeclUse);
    Rc<ast::Decl> decl = deref_decluse(llvm::cast<pt::DeclUse>(node));
    if (!decl->is_type()) {
      xerr("expected type, got {}", fmt::underlying(decl->get_kind()));
    }

    return std::static_pointer_cast<ast::Type>(decl);
  }

  Rc<ast::Decl> deref_decluse(pt::DeclUse& node) {
    if (auto itr = _decls.find(node.definition()); itr != _decls.end()) {
      return itr->second;
    }

    return lower_decl(_pt->get_node(node.definition()));
  }

  Ptr<ast::Block> lower_block(pt::NodeId node) {
    pt::Node& block = _pt->get_node(node);
    assert(block.kind() == pt::Node::Kind::Block);
    return lower_block(llvm::cast<pt::Block>(block));
  }

  Ptr<ast::Block> lower_block(pt::Block& node) {
    std::vector<Ptr<ast::Stmt>> stmts;
    std::vector<Rc<ast::Decl>> localvars;

    for (pt::NodeId stmtnodeid : node._body) {
      pt::Node& stmtnode = _pt->get_node(stmtnodeid);

      // vardecl is a decl and a stmt. we need to treat it as a decl, so we
      // check for those first
      if (stmtnode.is_decl()) {
        localvars.push_back(lower_decl(stmtnode));
      } else if (stmtnode.is_stmt()) {
        stmts.push_back(lower_stmt(stmtnode));
      } else {
        xerr("expected stmt, got {}", fmt::underlying(stmtnode.kind()));
      }

      // std::visit(overloaded{
      //                [this, &body](auto stmt) {
      //                  body.push_back(static_cast<ast::Stmt*>(check(stmt)));
      //                },
      //                [this, &body](pt::VarDecl* stmt) {
      //                  ast::Type* type = env.resolve_type(stmt->_type);
      //
      //                  auto* decl =
      //                      ast::VarDecl::Create(*_ast, stmt->_name, type);
      //                  body.push_back(decl);
      //
      //                  // we need to evaluate expression before adding
      //                  variable to
      //                      // env to avoid referencing itself
      //                      if (!stmt->_val.has_value()) {
      //                    env.add(decl);
      //                    return;
      //                  }
      //
      //                  ast::Expr* lhs = ast::VarRef::Create(*_ast, decl,
      //                  type); ast::Expr* val = check(*stmt->_val); auto*
      //                  assign = ast::Assign::Create(*_ast, lhs, val);
      //
      //                  body.push_back(assign);
      //                  env.add(decl);
      //                },
      //                [this, &body](pt::Expr& stmt) {
      //                  body.push_back(static_cast<ast::Stmt*>(check(stmt)));
      //                },
      //            },
      //            stmt);
    }

    return std::make_unique<ast::Block>(std::move(stmts), std::move(localvars));
  }

  Ptr<ast::Stmt> lower_stmt(pt::Node& node) {
    switch (node.kind()) {
      case pt::Node::Kind::Return: {
        auto& ret = llvm::cast<pt::Return>(node);
        Ptr<ast::Expr> retval =
            ret._retVal.has_value() ? lower_expr(*ret._retVal) : nullptr;

        return std::make_unique<ast::Return>(std::move(retval));
      }
      default:
        xerr("unable to lower stmt: {}", fmt::underlying(node.kind()));
    }
  }

  Ptr<ast::Expr> lower_expr(pt::NodeId nodeid) {
    pt::Node& node = _pt->get_node(nodeid);
    switch (node.kind()) {
      case pt::Node::Kind::Integer: {
        auto& intnode = llvm::cast<pt::Integer>(node);
        constexpr int base = 32;
        constexpr int radix = 10;
        return std::make_unique<ast::IntegerLiteral>(
            llvm::APInt(base, intnode._val, radix), _ast->_int32Ty);
      }
      case pt::Node::Kind::DeclUse: {
        auto& use = llvm::cast<pt::DeclUse>(node);
        Rc<ast::Decl> decl = deref_decluse(use);
        if (decl->get_kind() != ast::Decl::DeclKind::Var) {
          xerr("expected var, got {}", fmt::underlying(decl->get_kind()));
        }
        auto var = std::static_pointer_cast<ast::VarDecl>(std::move(decl));
        return std::make_unique<ast::VarRef>(var, var->type());
      }
      default:
        xerr("unable to lower expr: {}", fmt::underlying(node.kind()));
    }
  }

  std::unique_ptr<pt::Context> _pt;
  std::unique_ptr<ast::Context> _ast = std::make_unique<ast::Context>();

  std::map<pt::NodeId, Rc<ast::Decl>> _decls;
};

std::unique_ptr<ast::Context> lower_pt(std::unique_ptr<pt::Context> pt) {
  Data data(std::move(pt));
  return data.lower();
}

}  // namespace x
