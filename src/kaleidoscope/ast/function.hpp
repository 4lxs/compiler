#pragma once

#include <string>
#include <vector>

#include "../types.hpp"
#include "expr.hpp"

class PrototypeAST {
 public:
  PrototypeAST(std::string name, std::vector<std::string> args,
               SourceLocation start);

  std::ostream& dump(std::ostream& out);

 private:
  std::string _name;
  std::vector<std::string> _args;
  SourceLocation _start;
};

class FunctionAST {
 public:
  FunctionAST(std::unique_ptr<PrototypeAST> proto,
              std::unique_ptr<ExprAST> body);

  std::ostream& dump(std::ostream& out, size_t ind);

 private:
  std::unique_ptr<PrototypeAST> _proto;
  std::unique_ptr<ExprAST> _body;
};
