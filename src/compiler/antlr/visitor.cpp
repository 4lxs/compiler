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

  _module = _ctx->module(ast::Path(std::move(modulepath), projScope));

  return {};
}

std::any Visitor::visitFunction(parser::XParser::FunctionContext* ctx) {
  std::string name = ctx->name->getText();

  ast::FnProto prototype{
      std::move(name), parse_params(ctx->params()),
      ctx->ret == nullptr ? std::nullopt : std::optional(get_stub(ctx->ret))};

  auto func = std::make_unique<ast::Fn>(_module, std::move(prototype));

  _block = func.get();

  ast::Stub* stub = _module->get_stub(std::string{prototype.name});
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

  bool projScope = ctx->initSep == nullptr;

  return _module->get_stub(ast::Path{std::move(components), projScope});
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
