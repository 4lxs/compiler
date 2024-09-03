#include "function.hpp"

PrototypeAST::PrototypeAST(std::string name, std::vector<std::string> args,
                           SourceLocation start)
    : _name(std::move(name)), _args(std::move(args)), _start(start) {}

std::ostream& PrototypeAST::dump(std::ostream& out) {
  return out << fmt::format("{} ({}) -> ()", _name, _args.size());
}

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> proto,
                         std::unique_ptr<ExprAST> body)
    : _proto{std::move(proto)}, _body{std::move(body)} {}

std::ostream& FunctionAST::dump(std::ostream& out, size_t ind) {
  indent(out, ind) << fmt::format("FunctionAST: ");
  _proto->dump(out);
  ++ind;
  indent(out, ind) << "\nBody:";
  return _body ? _body->dump(out, ind) : out << "null\n";
}
