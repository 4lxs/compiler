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
      case pt::Node::Kind::VarDecl: {
        // NOTE: vardecl is a decl and possibly a stmt (if it has a value).
        // this only handles the decl part.
        auto& vardecl = llvm::cast<pt::VarDecl>(node);
        return decl = std::make_unique<ast::VarDecl>(vardecl.name(),
                                                     get_type(vardecl._type));
      }
      default:
        xerr("unable to lower decl {}", fmt::underlying(node.kind()));
    }
  }

  Rc<ast::LiteralTy> lower_primitive(pt::Primitive& node) {
    return _ast->_int32Ty = std::make_unique<ast::LiteralTy>(
               ast::LiteralTy::Kind::I32, node.name());
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

    auto oldfn = std::move(_currentFunction);
    _currentFunction = std::make_unique<CurrFn>();

    Ptr<ast::Block> body = lower_block(node._body);

    auto retFn = std::make_unique<ast::FnDecl>(
        node.name(), std::move(params), rettype, std::move(body),
        std::move(_currentFunction->localvars));

    _currentFunction = std::move(oldfn);

    return retFn;
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
    auto oldblock = std::move(_currentBlock);
    _currentBlock = std::make_unique<CurrBlock>();

    for (pt::NodeId stmtnodeid : node._body) {
      pt::Node& stmtnode = _pt->get_node(stmtnodeid);

      if (!stmtnode.is_decl() && !stmtnode.is_stmt()) {
        xerr("expected stmt or decl, got {}", fmt::underlying(stmtnode.kind()));
      }

      // NOTE: vardecl is a decl and a stmt. we need to declare it first, before
      // assigning to it
      if (stmtnode.is_decl()) {
        _currentFunction->localvars.push_back(lower_decl(stmtnode));
      }
      if (stmtnode.is_stmt()) {
        lower_stmt(stmtnode);
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

    auto ret = std::make_unique<ast::Block>(std::move(_currentBlock->stmts));
    _currentBlock = std::move(oldblock);
    return ret;
  }

  /// NOTE: it adds the stmt to the _currentBlock
  void lower_stmt(pt::Node& node) {
    if (node.is_expr()) {
      _currentBlock->stmts.push_back(lower_expr(node.id()));
      return;
    }
    switch (node.kind()) {
      case pt::Node::Kind::Return: {
        auto& ret = llvm::cast<pt::Return>(node);
        Ptr<ast::Expr> retval =
            ret._retVal.has_value() ? lower_expr(*ret._retVal) : nullptr;

        _currentBlock->stmts.push_back(
            std::make_unique<ast::Return>(std::move(retval)));
      } break;
      case pt::Node::Kind::VarDecl: {
        // NOTE: vardecl is a decl and a stmt. this only handles the stmt part
        auto& vardecl = llvm::cast<pt::VarDecl>(node);
        if (!vardecl._val.has_value()) {
          break;
        }

        Rc<ast::Decl> decl = lower_decl(vardecl);
        if (decl->get_kind() != ast::Decl::DeclKind::Var) {
          xerr("expected value decl, got {}",
               fmt::underlying(decl->get_kind()));
        }
        auto astVar = std::static_pointer_cast<ast::VarDecl>(std::move(decl));
        auto lhs = std::make_unique<ast::VarRef>(astVar, astVar->type());
        Ptr<ast::Expr> val = lower_expr(*vardecl._val);

        _currentBlock->stmts.push_back(
            ast::Builtin::CreateAssignment(std::move(lhs), std::move(val)));
      } break;
      case pt::Node::Kind::While: {
        auto& whilenode = llvm::cast<pt::While>(node);

        auto looplabel = std::make_shared<ast::Label>();

        auto loopTest = [&]() {
          Ptr<ast::Expr> loopTestCond =
              ast::Builtin::CreateNot(lower_expr(whilenode._cond));
          auto breakstmt = std::make_unique<ast::Return>(nullptr, looplabel);
          std::vector<Ptr<ast::Stmt>> stmts;
          stmts.push_back(std::move(breakstmt));
          auto breakblock = std::make_unique<ast::Block>(std::move(stmts));
          auto loopTest = std::make_unique<ast::If>(
              std::move(loopTestCond), std::move(breakblock), nullptr);

          return loopTest;
        }();

        Ptr<ast::Block> body = lower_block(whilenode._body);
        body->_label = looplabel;
        looplabel->_block = body.get();

        std::vector<Ptr<ast::Stmt>> stmts;
        stmts.reserve(body->_body.size() + 1);
        stmts.push_back(std::move(loopTest));
        stmts.insert(stmts.end(), std::make_move_iterator(body->_body.begin()),
                     std::make_move_iterator(body->_body.end()));
        body->_body = std::move(stmts);

        _currentBlock->stmts.push_back(
            std::make_unique<ast::Loop>(std::move(body)));
      } break;
      case pt::Node::Kind::Assign: {
        auto& assignNode = llvm::cast<pt::Assign>(node);

        Ptr<ast::Expr> lhs = lower_expr(assignNode._assignee);
        Ptr<ast::Expr> rhs = lower_expr(assignNode._value);

        _currentBlock->stmts.push_back(
            ast::Builtin::CreateAssignment(std::move(lhs), std::move(rhs)));
      } break;
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
      case pt::Node::Kind::If: {
        auto& ifnode = llvm::cast<pt::IfExpr>(node);
        Ptr<ast::Expr> cond = lower_expr(ifnode.cond);
        Ptr<ast::Block> then = lower_block(ifnode.then);
        Ptr<ast::Block> els;
        if (ifnode.else_.has_value()) {
          els = lower_block(ifnode.else_.value());
        }
        return std::make_unique<ast::If>(std::move(cond), std::move(then),
                                         std::move(els));
      }
      case pt::Node::Kind::Binary: {
        auto& binnode = llvm::cast<pt::BinaryNode>(node);
        std::vector<Ptr<ast::Expr>> args;
        args.push_back(lower_expr(binnode.l));
        args.push_back(lower_expr(binnode.r));

        using PtOp = pt::BinaryNode::Operator;
        using AstOp = ast::Builtin::Op;
        std::map<PtOp, AstOp> table{
            {PtOp::Less, AstOp::iLess}, {PtOp::Greater, AstOp::iGreater},
            {PtOp::Plus, AstOp::iAdd},  {PtOp::Minus, AstOp::iSub},
            {PtOp::Star, AstOp::iMul},  {PtOp::Slash, AstOp::iDiv},
        };
        return std::make_unique<ast::Builtin>(
            table.at(binnode.op), std::move(args), args.front()->type());
      }
      default:
        xerr("unable to lower expr: {}", fmt::underlying(node.kind()));
    }
  }

  std::unique_ptr<pt::Context> _pt;
  std::unique_ptr<ast::Context> _ast = std::make_unique<ast::Context>();

  std::map<pt::NodeId, Rc<ast::Decl>> _decls;

  // NOTE: a global fn may be called from within another fn
  struct CurrFn {
    std::vector<Rc<ast::Decl>> localvars;
  };
  Ptr<CurrFn> _currentFunction;

  struct CurrBlock {
    std::vector<Ptr<ast::Stmt>> stmts;
  };
  Ptr<CurrBlock> _currentBlock;
};

std::unique_ptr<ast::Context> lower_pt(std::unique_ptr<pt::Context> pt) {
  Data data(std::move(pt));
  return data.lower();
}

}  // namespace x
