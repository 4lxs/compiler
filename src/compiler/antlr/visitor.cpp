#include "visitor.hpp"

#include <any>
#include <ranges>

#include "XParser.h"
#include "spdlog/spdlog.h"
#include "x/pt/decl.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/module.hpp"
#include "x/pt/stmt.hpp"

namespace x {

Visitor::Visitor(pt::Context* ctx) : _ctx{ctx} {}

std::any Visitor::visitModdef(parser::XParser::ModdefContext* ctx) {
  std::vector<std::string> modulepath;

  for (auto* ident : ctx->Ident()) {
    modulepath.emplace_back(ident->toString());
  }

  bool external = ctx->initSep != nullptr;

  _module = _ctx->module(pt::Path(std::move(modulepath), external));

  return {};
}

std::any Visitor::visitFunction(parser::XParser::FunctionContext* ctx) {
  visitBlock(ctx->block());
  auto* body = std::get<pt::Block*>(_stack.pop());

  auto* retType = pt::DeclRef::Create(*_ctx, get_path(ctx->ret));

  auto [params, isStatic] = [&, ctx = ctx->params()]() {
    auto parse_param = [&](parser::XParser::ParamContext* param) {
      auto* name = param->Ident();
      auto* type = param->path();
      auto* typeref = pt::DeclRef::Create(*_ctx, get_path(type));
      return pt::FnParam{name->getText(), typeref};
    };

    std::vector<pt::FnParam> params;
    std::vector<parser::XParser::ParamContext*> ctxParams = ctx->param();
    params.reserve(ctxParams.size());
    for (auto* param : ctxParams) {
      params.emplace_back(parse_param(param));
    }

    bool isStatic = true;
    if (ctx->self != nullptr) {
      assert(ctx->self->getText() == "this");
      isStatic = false;
    }

    return std::make_pair(params, isStatic);
  }();

  if (ctx->class_ != nullptr) {
    pt::DeclRef* recv = pt::DeclRef::Create(*_ctx, get_path(ctx->class_));
    auto* method =
        pt::MethodDecl::Create(*_ctx, recv, ctx->name->getText(),
                               std::move(params), retType, body, isStatic);

    _module->define(method);

    return {};
  }

  assert(isStatic);

  auto* func = pt::FnDecl::Create(*_ctx, ctx->name->getText(),
                                  std::move(params), retType, body);

  _module->define(func);

  return {};
}

std::any Visitor::visitReturn(parser::XParser::ReturnContext* ctx) {
  assert(_block != nullptr);

  auto* retVal = ctx->expr();
  if (retVal == nullptr) {
    _block->add(pt::Return::Create(*_ctx, std::nullopt));
  } else {
    visit(retVal);

    _block->add(pt::Return::Create(*_ctx, _stack.pop()));
  }

  return {};
}

std::any Visitor::visitCallE(parser::XParser::CallEContext* ctx) {
  assert(_block != nullptr);

  auto* fnVar = pt::DeclRef::Create(*_ctx, get_path(ctx->fn));

  visitStructExpr(ctx->structExpr());
  auto* args = std::get<pt::StructExpr*>(_stack.pop());

  _stack.push(pt::Call::Create(*_ctx, fnVar, args));

  return {};
}

std::any Visitor::visitIntPE(parser::XParser::IntPEContext* ctx) {
  _stack.push(pt::IntegerE::Create(*_ctx, ctx->getText()));

  return {};
}

std::any Visitor::visitBinaryE(parser::XParser::BinaryEContext* ctx) {
  visitChildren(ctx);
  auto exprs = _stack.pop(2);

  using Op = pt::BinaryExpr::Operator;
  using Tok = parser::XParser;
  Op opr{};

  switch (ctx->bop->getType()) {
    case Tok::Plus:
      opr = Op::Plus;
      break;
    case Tok::Minus:
      opr = Op::Minus;
      break;
    case Tok::Star:
      opr = Op::Star;
      break;
    case Tok::Slash:
      opr = Op::Slash;
      break;
    case Tok::Greater:
      opr = Op::Greater;
      break;
    case Tok::Less:
      opr = Op::Less;
      break;
    default:
      spdlog::error("unexpected token");
      std::terminate();
  }

  _stack.push(pt::BinaryExpr::Create(*_ctx, exprs[0], exprs[1], opr));

  return {};
}

std::any Visitor::visitStructExpr(parser::XParser::StructExprContext* ctx) {
  auto ctxFields = ctx->structExprField();

  visitChildren(ctx);
  std::vector<pt::Expr> vals = _stack.pop(ctxFields.size());
  std::vector<pt::Field> fields;
  fields.reserve(vals.size());

  for (auto&& [fieldCtx, expr] : std::views::zip(ctxFields, vals)) {
    std::string name =
        (fieldCtx->Ident() != nullptr) ? fieldCtx->Ident()->getText() : "";
    fields.emplace_back(std::move(name), expr);
  }

  _stack.push(pt::StructExpr::Create(*_ctx, std::move(fields)));

  return {};
}

std::any Visitor::visitStmt(parser::XParser::StmtContext* ctx) {
  visitChildren(ctx);

  if (_stack.size() > 0) {
    _block->add(_stack.pop());
  }

  assert(_stack.size() == 0);

  return {};
}

std::any Visitor::visitIf_(parser::XParser::If_Context* ctx) {
  visit(ctx->expr());

  pt::Expr cond = _stack.pop();

  visitBlock(ctx->then);

  not_null<pt::Block*> then = std::get<pt::Block*>(_stack.pop());

  pt::Block* else_{};

  if (ctx->else_ != nullptr) {
    visitBlock(ctx->else_);

    else_ = std::get<pt::Block*>(_stack.pop());
  }

  _stack.push(pt::IfExpr::Create(*_ctx, cond, then, else_));

  return {};
}

std::any Visitor::visitBlock(parser::XParser::BlockContext* ctx) {
  pt::Block* oldblock = _block;

  _block = pt::Block::Create(*_ctx);

  visitChildren(ctx);

  if (ctx->terminator != nullptr) {
    _block->setTerminator(_stack.pop());
  }

  _stack.push(_block);

  _block = oldblock;

  return {};
}

std::any Visitor::visitVarDef(parser::XParser::VarDefContext* ctx) {
  std::string name = ctx->name->getText();

  std::optional<pt::Expr> val;

  if (ctx->val != nullptr) {
    visit(ctx->val);
    val = _stack.pop();
  }

  auto* type = pt::DeclRef::Create(*_ctx, get_path(ctx->typ));

  _block->add(pt::VarDecl::Create(*_ctx, std::move(name), type, val));

  return {};
}

std::any Visitor::visitVarAssign(parser::XParser::VarAssignContext* ctx) {
  visit(ctx->primaryExpr());
  pt::Expr assignee = _stack.pop();
  visit(ctx->expr());
  pt::Expr val = _stack.pop();

  auto* expr = pt::Assign::Create(*_ctx, assignee, val);
  _block->add(expr);

  return {};
}

std::any Visitor::visitVarE(parser::XParser::VarEContext* ctx) {
  _stack.push(pt::DeclRef::Create(*_ctx, pt::Path({ctx->Ident()->getText()})));

  return {};
}

std::any Visitor::visitStructDef(parser::XParser::StructDefContext* ctx) {
  std::vector<pt::StructDecl::Field> fields;

  for (parser::XParser::StructFieldContext* field : ctx->structField()) {
    fields.push_back(parse_field(field));
  }

  _module->define(
      pt::StructDecl::Create(*_ctx, ctx->name->getText(), std::move(fields)));

  return {};
}

std::any Visitor::visitWhile(parser::XParser::WhileContext* ctx) {
  visit(ctx->expr());
  pt::Expr cond = _stack.pop();
  visitBlock(ctx->block());
  pt::Block* body = std::get<pt::Block*>(_stack.pop());

  _block->add(pt::While::Create(*_ctx, cond, body));

  return {};
}

std::any Visitor::visitEnumDef(parser::XParser::EnumDefContext* ctx) {
  std::vector<pt::EnumDecl::Variant> variants;

  for (auto* variant : ctx->enumVariant()) {
    variants.push_back(parse_variant(variant));
  }

  _module->define(
      pt::EnumDecl::Create(*_ctx, ctx->name->getText(), std::move(variants)));

  return {};
}

pt::EnumDecl::Variant Visitor::parse_variant(
    parser::XParser::EnumVariantContext* ctx) {
  return pt::EnumDecl::Variant{.name = ctx->name->getText()};
}

std::any Visitor::visitTypeDef(parser::XParser::TypeDefContext* ctx) {
  pt::TypeDecl* decl =
      pt::TypeDecl::Create(*_ctx, ctx->name->getText(),
                           pt::DeclRef::Create(*_ctx, get_path(ctx->path())));

  _module->define(decl);

  return {};
}

std::any Visitor::visitMemberAccess(parser::XParser::MemberAccessContext* ctx) {
  pt::Expr base = _stack.pop();

  pt::Expr expr = pt::FieldAccess::Create(*_ctx, base, ctx->field->getText());

  if (ctx->args != nullptr) {
    std::terminate();

    // visitStructExpr(ctx->args);
    // expr =
    //     pt::Call::Create(*_ctx, expr,
    //     std::get<pt::StructExpr*>(_stack.pop()));
  }

  _stack.push(expr);

  return {};
}

pt::StructDecl::Field Visitor::parse_field(
    parser::XParser::StructFieldContext* ctx) {
  std::optional<pt::Expr> defVal;
  if (ctx->value != nullptr) {
    visit(ctx->value);
    defVal = _stack.pop();
  }

  return pt::StructDecl::Field{
      .name = ctx->name->getText(),
      .type = pt::DeclRef::Create(*_ctx, get_path(ctx->path())),
      .defaultVal = defVal,
  };
}

pt::Path Visitor::get_path(parser::XParser::PathContext* ctx) {
  std::vector<std::string> components;

  for (auto* ident : ctx->Ident()) {
    components.emplace_back(ident->toString());
  }

  bool external = ctx->initSep != nullptr;

  return pt::Path{std::move(components), external};
}

}  // namespace x
