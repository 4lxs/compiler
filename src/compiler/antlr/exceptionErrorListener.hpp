#pragma once

#include <BaseErrorListener.h>

/*
 * alternative error listener that throw the error message
 */
class ExceptionErrorListener : public antlr4::BaseErrorListener {
 public:
  void syntaxError(antlr4::Recognizer * /*recognizer*/,
                   antlr4::Token * /*offendingSymbol*/, size_t line,
                   size_t charPositionInLine, std::string const &msg,
                   std::exception_ptr eptr) override {
    eptr = nullptr;
    std::ostringstream oss;
    oss << "line: " << line << ":" << charPositionInLine << " " << msg;
    error_msg = oss.str();
    throw antlr4::ParseCancellationException(error_msg);
  }

 private:
  std::string error_msg;
};
