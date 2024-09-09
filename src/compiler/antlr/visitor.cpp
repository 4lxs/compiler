#include "visitor.hpp"

#include <any>
#include <memory>
#include <ranges>

#include "XParser.h"
#include "x/ast/expr.hpp"

namespace x {

Visitor::Visitor(ast::Context* ctx) : _ctx{ctx} {}

std::any Visitor::visitModdef(parser::XParser::ModdefContext* ctx) {
  std::vector<std::string> modulepath;

  for (auto* ident : ctx->Ident()) {
    modulepath.emplace_back(ident->toString());
  }

  bool external = ctx->initSep != nullptr;
  spdlog::info("external? {}", external);

  _module = _ctx->module(ast::Path(std::move(modulepath), external));

  return {};
}

std::any Visitor::visitFunction(parser::XParser::FunctionContext* ctx) {
  std::string name = ctx->name->getText();

  ast::Stub* retStub = ctx->ret != nullptr ? get_stub(ctx->ret) : nullptr;

  ast::FnProto prototype{std::move(name), parse_params(ctx->params()), retStub};

  auto func = std::make_unique<ast::Fn>(_module, std::move(prototype));

  _block = func.get();

  ast::Stub* stub = _module->get_stub(std::string{func->name()});
  stub->define_function(std::move(func));

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

  _block->call(get_stub(ctx->fn), expr.into<ast::StructExpr>());

  return {};
}

std::any Visitor::visitIntPE(parser::XParser::IntPEContext* ctx) {
  _stack.push(ast::PrimaryExpr::Int(ctx->getText()));

  return {};
}

std::any Visitor::visitBinaryE(parser::XParser::BinaryEContext* ctx) {
  visitChildren(ctx);
  auto exprs = _stack.pop(2);

  using Op = ast::BinaryExpr::Operator;
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

  _stack.push(std::make_unique<ast::BinaryExpr>(std::move(exprs[0]),
                                                std::move(exprs[1]), opr));

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

std::vector<ast::FnParam> Visitor::parse_params(
    parser::XParser::ParamsContext* ctx) {
  auto names = ctx->Ident();
  auto types = ctx->path();
  assert(names.size() == types.size());

  std::vector<ast::FnParam> params;
  params.reserve(names.size());
  for (auto&& [name, type] : std::views::zip(names, types)) {
    params.emplace_back(name->getText(), get_stub(type));
  }

  return params;
}

ast::Stub* Visitor::get_stub(parser::XParser::PathContext* ctx) const {
  std::vector<std::string> components;

  for (auto* ident : ctx->Ident()) {
    components.emplace_back(ident->toString());
  }

  bool external = ctx->initSep != nullptr;
  spdlog::info("external? {}", external);

  return _module->get_stub(ast::Path{std::move(components), external});
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
