#include "visitor.hpp"

#include <any>
#include <memory>
#include <ranges>

namespace x {

Visitor::Visitor(ast::Context* ctx) : _ctx{ctx} {}

std::any Visitor::visitModdef(parser::XParser::ModdefContext* ctx) {
  std::vector<std::string> modulepath;

  for (auto* ident : ctx->Ident()) {
    modulepath.emplace_back(ident->toString());
  }

  bool projScope = ctx->initSep == nullptr;

  _module = _ctx->module(std::move(modulepath), projScope);

  return {};
}

std::any Visitor::visitFunction(parser::XParser::FunctionContext* ctx) {
  ast::FnProto prototype{
      ctx->name->getText(), parseParams(ctx->params()),
      ctx->ret == nullptr ? std::nullopt : std::optional(getType(ctx->ret))};

  ast::Fn* func = _module->function(std::move(prototype));
  _block = func;

  visitBlock(ctx->block());
  return {};
}

std::any Visitor::visitReturn(parser::XParser::ReturnContext* ctx) {
  assert(_block != nullptr);

  auto* retVal = ctx->expr();
  if (retVal == nullptr) {
    _block->ret();
  } else {
    visit(retVal);

    _block->ret(_stack.pop());
  }

  return {};
}

std::any Visitor::visitCallE(parser::XParser::CallEContext* ctx) {
  visitAnonStruct(ctx->anonStruct());

  assert(_block != nullptr);

  ast::Expr expr = _stack.pop();

  _block->call(getPath(ctx->fn), expr.into<ast::StructExpr>());

  return {};
}

std::any Visitor::visitIntPE(parser::XParser::IntPEContext* ctx) {
  _stack.push(ast::PrimaryExpr::Int(ctx->getText()));

  return {};
}

std::any Visitor::visitAnonStruct(parser::XParser::AnonStructContext* ctx) {
  visitChildren(ctx);

  std::vector<antlr4::tree::TerminalNode*> names = ctx->Ident();
  std::vector<ast::Expr> vals = _stack.pop(names.size());
  std::vector<ast::Field> fields;
  fields.reserve(names.size());

  for (auto&& [ident, expr] : std::views::zip(names, vals)) {
    fields.emplace_back(ident->getText(), std::move(expr));
  }

  _stack.push(ast::StructExpr::Create(std::move(fields)));

  return {};
}

std::vector<ast::FnParam> Visitor::parseParams(
    parser::XParser::ParamsContext* ctx) {
  auto names = ctx->Ident();
  auto types = ctx->path();
  assert(names.size() == types.size());

  std::vector<ast::FnParam> params;
  params.reserve(names.size());
  for (auto&& [name, type] : std::views::zip(names, types)) {
    params.emplace_back(name->getText(), getType(type));
  }

  return params;
}

ast::Type* Visitor::getType(parser::XParser::PathContext* ctx) const {
  return _module->type(getPath(ctx));
}

ast::Path* Visitor::getPath(parser::XParser::PathContext* ctx) const {
  std::vector<std::string> components;

  for (auto* ident : ctx->Ident()) {
    components.emplace_back(ident->toString());
  }

  bool projScope = ctx->initSep == nullptr;

  return _module->path(std::move(components), projScope);
}

ast::Expr Visitor::Stack_::pop() {
  assert(!_stack.empty());

  auto ret = std::move(_stack.back());
  _stack.pop_back();
  return ret;
}

std::vector<ast::Expr> Visitor::Stack_::pop(size_t cnt) {
  assert(_stack.size() >= cnt);

  auto start = _stack.end() - ssize_t(cnt);
  auto end = _stack.end();
  std::vector<ast::Expr> ret(std::move_iterator{start},
                             std::move_iterator{end});

  _stack.erase(start, end);

  return ret;
}

}  // namespace x
