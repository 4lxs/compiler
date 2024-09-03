#include <iostream>
#include <string_view>

// #include "TBaseListener.h"
#include "TLexer.h"
#include "TParser.h"
#include "antlr4-runtime.h"

using namespace parser;
using namespace antlr4;

// class TreeShapeListener : public parser::{
//  public:
//   void enterKey(ParserRuleContext *ctx) override {
//     // Do something when entering the key rule.
//   }
// };

int main(int, const char **) {
  ANTLRInputStream input(std::cin);
  TLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  TParser parser(&tokens);
  tree::ParseTree *tree = parser.main();

  std::cout << tree->toStringTree(&parser, true) << std::endl << std::endl;

  return 0;
}
