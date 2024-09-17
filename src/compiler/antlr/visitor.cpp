#include "visitor.hpp"

#include <any>
#include <memory>
#include <ranges>

#include "XParser.h"
#include "spdlog/spdlog.h"
#include "x/ast/expr.hpp"
#include "x/ast/stmt.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/module.hpp"

namespace x {

Visitor::Visitor(pt::Context* ctx) : _ctx{ctx} {}

std::any Visitor::visitModdef(parser::XParser::ModdefContext* ctx) {
  std::vector<std::string> modulepath;

  for (auto* ident : ctx->Ident()) {
    modulepath.emplace_back(ident->toString());
  }

  bool external = ctx->initSep != nullptr;
  spdlog::info("external? {}", external);

  _module = _ctx->module(pt::Path(std::move(modulepath), external));

  return {};
}

std::any Visitor::visitFunction(parser::XParser::FunctionContext* ctx) {
  pt::Type* retType =
      ctx->ret != nullptr ? get_stub(ctx->ret)->use_type() : nullptr;

  pt::FnProto prototype{parse_params(ctx->params()), retType};

  pt::Stub* stub = _module->get_stub(ctx->name->getText());

  visitBlock(ctx->block());

  stub->function(std::move(prototype),
                 std::move(std::get<Ptr<pt::Block>>(_stack.pop())));

  return {};
}

std::any Visitor::visitReturn(parser::XParser::ReturnContext* ctx) {
  assert(_block != nullptr);

  auto* retVal = ctx->expr();
  if (retVal == nullptr) {
    _block->ret(std::nullopt);
  } else {
    visit(retVal);

    _block->ret(_stack.pop());
  }

  return {};
}

std::any Visitor::visitCallE(parser::XParser::CallEContext* ctx) {
  visitAnonStruct(ctx->anonStruct());

  assert(_block != nullptr);

  Ptr<pt::StructExpr> args = std::get<Ptr<pt::StructExpr>>(_stack.pop());
  pt::Fn* func = get_stub(ctx->fn)->function(*args);

  _stack.push(std::make_unique<pt::Call>(func, std::move(args)));

  return {};
}

std::any Visitor::visitIntPE(parser::XParser::IntPEContext* ctx) {
  _stack.push(std::make_unique<pt::IntegerE>(ctx->getText()));

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

  _stack.push(std::make_unique<pt::BinaryExpr>(std::move(exprs[0]),
                                               std::move(exprs[1]), opr));

  return {};
}

std::any Visitor::visitAnonStruct(parser::XParser::AnonStructContext* ctx) {
  visitChildren(ctx);

  std::vector<antlr4::tree::TerminalNode*> names = ctx->Ident();
  std::vector<pt::Expr> vals = _stack.pop(names.size());
  std::vector<pt::Field> fields;
  fields.reserve(names.size());

  for (auto&& [ident, expr] : std::views::zip(names, vals)) {
    fields.emplace_back(ident->getText(), std::move(expr));
  }

  _stack.push(pt::StructExpr::Create(std::move(fields)));

  return {};
}

std::any Visitor::visitStmt(parser::XParser::StmtContext* ctx) {
  visitChildren(ctx);

  if (_stack.size() > 0) {
    _block->expr(_stack.pop());
  }

  assert(_stack.size() == 0);

  return {};
}

std::any Visitor::visitIf_(parser::XParser::If_Context* ctx) {
  visit(ctx->expr());

  pt::Expr cond = _stack.pop();

  visitBlock(ctx->then);

  Ptr<pt::Block> then = std::get<Ptr<pt::Block>>(_stack.pop());

  Ptr<pt::Block> else_;

  if (ctx->else_ != nullptr) {
    visitBlock(ctx->else_);

    else_ = std::get<Ptr<pt::Block>>(_stack.pop());
  }

  _stack.push(std::make_unique<pt::IfExpr>(std::move(cond), std::move(then),
                                           std::move(else_)));

  return {};
}

std::any Visitor::visitBlock(parser::XParser::BlockContext* ctx) {
  pt::Block* oldblock = _block;

  auto block = std::make_unique<pt::Block>();
  _block = block.get();

  visitChildren(ctx);

  if (ctx->terminator != nullptr) {
    block->setTerminator(_stack.pop());
  }

  _block = oldblock;

  _stack.push(std::move(block));

  return {};
}

std::vector<pt::FnParam> Visitor::parse_params(
    parser::XParser::ParamsContext* ctx) {
  auto names = ctx->Ident();
  auto types = ctx->path();
  assert(names.size() == types.size());

  std::vector<pt::FnParam> params;
  params.reserve(names.size());
  for (auto&& [name, type] : std::views::zip(names, types)) {
    params.emplace_back(name->getText(), get_stub(type)->use_type());
  }

  return params;
}

pt::Stub* Visitor::get_stub(parser::XParser::PathContext* ctx) const {
  std::vector<std::string> components;

  for (auto* ident : ctx->Ident()) {
    components.emplace_back(ident->toString());
  }

  bool external = ctx->initSep != nullptr;
  spdlog::info("external? {}", external);

  return _module->get_stub(pt::Path{std::move(components), external});
}

pt::Expr Visitor::Stack_::pop() {
  assert(!_stack.empty());

  auto ret = std::move(_stack.back());
  _stack.pop_back();
  return ret;
}

std::vector<pt::Expr> Visitor::Stack_::pop(size_t cnt) {
  assert(_stack.size() >= cnt);

  auto start = _stack.end() - ssize_t(cnt);
  auto end = _stack.end();
  std::vector<pt::Expr> ret(std::move_iterator{start}, std::move_iterator{end});

  _stack.erase(start, end);

  return ret;
}

}  // namespace x
