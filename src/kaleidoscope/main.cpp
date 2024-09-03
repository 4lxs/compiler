#include <spdlog/spdlog.h>

#include <cctype>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

#include "generator.hpp"
#include "parser.hpp"
#include "scanner.hpp"

int main() {
  std::ifstream file("file.x");
  auto scanner = std::make_unique<Scanner>(std::string{
      std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()});

  Parser parser(std::move(scanner));

  Generator gen;

  while (parser.parse_toplevel(gen)) {
    spdlog::info("got ast:");
  }
  return 0;
}
