#include "visitor.hpp"

#include <any>
#include <ranges>

#include "XParser.h"
#include "spdlog/spdlog.h"
#include "x/pt/block.hpp"
#include "x/pt/decl.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/stmt.hpp"

namespace x {

Visitor::Visitor(pt::Context* ctx) : _ctx{ctx} {}

// std::any Visitor::visitModdef(parser::XParser::ModdefContext* ctx) {
//   std::vector<std::string> modulepath;
//
//   for (auto* ident : ctx->Ident()) {
//     modulepath.emplace_back(ident->toString());
//   }
//
//   bool external = ctx->initSep != nullptr;
//
//   _module = _ctx->module(pt::Path(std::move(modulepath), external));
//
//   return {};
// }

std::any Visitor::visitFunction(parser::XParser::FunctionContext* ctx) {
  visitBlock(ctx->block());
  pt::NodeId body = _stack.pop();

  auto& retType = _ctx->create<pt::DeclUse>(get_path(ctx->ret));

  auto [params, isStatic] = [&, ctx = ctx->params()]() {
    auto parse_param = [&](parser::XParser::ParamContext* param) {
      auto* name = param->Ident();
      auto* type = param->path();
      auto& typeref = _ctx->create<pt::DeclUse>(get_path(type));
      return _ctx->create<pt::ParamDecl>(name->getText(), typeref.id()).id();
    };

    std::vector<pt::NodeId> params;
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
    pt::NodeId recv = _ctx->create<pt::DeclUse>(get_path(ctx->class_)).id();
    auto& method = _ctx->create<pt::MethodDecl>(recv, ctx->name->getText(),
                                                std::move(params), retType.id(),
                                                body, isStatic);

    _ctx->add_item(method.id());

    return {};
  }

  assert(isStatic);

  auto& func = _ctx->create<pt::FnDecl>(ctx->name->getText(), std::move(params),
                                        retType.id(), body);

  _ctx->add_item(func.id());

  return {};
}

std::any Visitor::visitReturn(parser::XParser::ReturnContext* ctx) {
  assert(_block != nullptr);

  auto* retVal = ctx->expr();
  std::optional<pt::NodeId> ret;
  if (retVal != nullptr) {
    visit(retVal);
    ret = _stack.pop();
  }

  _block->add(_ctx->create<pt::Return>(ret).id());

  return {};
}

std::any Visitor::visitCallE(parser::XParser::CallEContext* ctx) {
  assert(_block != nullptr);

  visit(ctx->fn);
  pt::NodeId fnVar = _stack.pop();

  visitStructExpr(ctx->structExpr());
  pt::NodeId args = _stack.pop();

  _stack.push(_ctx->create<pt::Call>(fnVar, args).id());

  return {};
}

std::any Visitor::visitIntPE(parser::XParser::IntPEContext* ctx) {
  _stack.push(_ctx->create<pt::Integer>(ctx->getText()).id());

  return {};
}

std::any Visitor::visitBinaryE(parser::XParser::BinaryEContext* ctx) {
  visitChildren(ctx);
  auto exprs = _stack.pop(2);

  using Op = pt::BinaryNode::Operator;
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

  _stack.push(_ctx->create<pt::BinaryNode>(exprs[0], exprs[1], opr).id());

  return {};
}

std::any Visitor::visitStructExpr(parser::XParser::StructExprContext* ctx) {
  auto ctxFields = ctx->structExprField();

  visitChildren(ctx);
  std::vector<pt::NodeId> vals = _stack.pop(ctxFields.size());
  std::vector<pt::Field> fields;
  fields.reserve(vals.size());

  for (auto&& [fieldCtx, expr] : std::views::zip(ctxFields, vals)) {
    std::string name =
        (fieldCtx->Ident() != nullptr) ? fieldCtx->Ident()->getText() : "";
    fields.emplace_back(std::move(name), expr);
  }

  _stack.push(_ctx->create<pt::StructExpr>(std::move(fields)).id());

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

  pt::NodeId cond = _stack.pop();

  visitBlock(ctx->then);

  pt::NodeId then = _stack.pop();

  std::optional<pt::NodeId> else_;

  if (ctx->else_) {
    visitBlock(ctx->else_);

    else_ = _stack.pop();
  }

  _stack.push(_ctx->create<pt::IfExpr>(cond, then, else_).id());

  return {};
}

std::any Visitor::visitBlock(parser::XParser::BlockContext* ctx) {
  pt::Block* oldblock = _block;

  _block = &_ctx->create<pt::Block>();

  visitChildren(ctx);

  if (ctx->terminator != nullptr) {
    _block->setTerminator(_stack.pop());
  }

  _stack.push(_block->id());

  _block = oldblock;

  return {};
}

std::any Visitor::visitVarDef(parser::XParser::VarDefContext* ctx) {
  std::string name = ctx->name->getText();

  std::optional<pt::NodeId> val;

  if (ctx->val != nullptr) {
    visit(ctx->val);
    val = _stack.pop();
  }

  pt::NodeId type = _ctx->create<pt::DeclUse>(get_path(ctx->typ)).id();

  _block->add(_ctx->create<pt::VarDecl>(std::move(name), type, val).id());

  return {};
}

std::any Visitor::visitVarAssign(parser::XParser::VarAssignContext* ctx) {
  visit(ctx->primaryExpr());
  pt::NodeId assignee = _stack.pop();
  visit(ctx->expr());
  pt::NodeId val = _stack.pop();

  pt::NodeId expr = _ctx->create<pt::Assign>(assignee, val).id();
  _block->add(expr);

  return {};
}

std::any Visitor::visitVarE(parser::XParser::VarEContext* ctx) {
  _stack.push(
      _ctx->create<pt::DeclUse>(pt::Path({ctx->Ident()->getText()})).id());

  return {};
}

std::any Visitor::visitStructDef(parser::XParser::StructDefContext* ctx) {
  std::vector<pt::StructDecl::Field> fields;

  for (parser::XParser::StructFieldContext* field : ctx->structField()) {
    fields.push_back(parse_field(field));
  }

  _ctx->add_item(
      _ctx->create<pt::StructDecl>(ctx->name->getText(), std::move(fields))
          .id());

  return {};
}

std::any Visitor::visitWhile(parser::XParser::WhileContext* ctx) {
  visit(ctx->expr());
  pt::NodeId cond = _stack.pop();
  visitBlock(ctx->block());
  pt::NodeId body = _stack.pop();

  _block->add(_ctx->create<pt::While>(cond, body).id());

  return {};
}

std::any Visitor::visitEnumDef(parser::XParser::EnumDefContext* ctx) {
  std::vector<pt::EnumDecl::Variant> variants;

  for (auto* variant : ctx->enumVariant()) {
    variants.push_back(parse_variant(variant));
  }

  _ctx->add_item(
      _ctx->create<pt::EnumDecl>(ctx->name->getText(), std::move(variants))
          .id());

  return {};
}

pt::EnumDecl::Variant Visitor::parse_variant(
    parser::XParser::EnumVariantContext* ctx) {
  return pt::EnumDecl::Variant{.name = ctx->name->getText()};
}

std::any Visitor::visitTypeDef(parser::XParser::TypeDefContext* ctx) {
  pt::NodeId use = _ctx->create<pt::DeclUse>(get_path(ctx->path())).id();

  pt::NodeId decl = _ctx->create<pt::TypeDecl>(ctx->name->getText(), use).id();

  _ctx->add_item(decl);

  return {};
}

std::any Visitor::visitMemberE(parser::XParser::MemberEContext* ctx) {
  visit(ctx->primaryExpr());
  pt::NodeId base = _stack.pop();

  pt::NodeId field =
      _ctx->create<pt::FieldAccess>(base, ctx->Ident()->getText()).id();

  _stack.push(field);

  return {};
}

pt::StructDecl::Field Visitor::parse_field(
    parser::XParser::StructFieldContext* ctx) {
  std::optional<pt::NodeId> defVal;
  if (ctx->value != nullptr) {
    visit(ctx->value);
    defVal = _stack.pop();
  }

  return pt::StructDecl::Field{
      .name = ctx->name->getText(),
      .type = _ctx->create<pt::DeclUse>(get_path(ctx->path())).id(),
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
