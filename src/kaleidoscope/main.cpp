#include <llvm/ADT/APFloat.h>

#include <iostream>

using namespace llvm;

constexpr float FLOAT = 3.14F;

int main() {
  const APFloat val{FLOAT};
  (void)val;

  std::cout << "hellyo\n";

  return 0;
}
