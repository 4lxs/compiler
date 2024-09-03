#---
# Antlr
#---
message(STATUS "adding antlr ${CMAKE_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/antlr)
add_definitions(-DANTLR4CPP_STATIC)

include(ExternalAntlr4Cpp)
include_directories(${ANTLR4_INCLUDE_DIRS})
find_package(ANTLR REQUIRED)

antlr_target(
  SampleGrammarLexer
  TLexer.g4
  LEXER
  PACKAGE
  parser)
antlr_target(
  SampleGrammarParser
  TParser.g4
  PARSER
  PACKAGE
  parser
  DEPENDS_ANTLR
  SampleGrammarLexer
  COMPILE_FLAGS
  -lib
  ${ANTLR_SampleGrammarLexer_OUTPUT_DIR})
# antlr_target(
#   Listener
#   TLexer.g4
#   TParser.g4
#   LISTENER
#   PACKAGE
#   parser
#   DEPENDS_ANTLR
#   SampleGrammarLexer
#   SampleGrammarParser
#   COMPILE_FLAGS
#   -lib
#   ${ANTLR_SampleGrammarLexer_OUTPUT_DIR}
#   -lib
#   ${ANTLR_SampleGrammarParser_OUTPUT_DIR})

#---
# Parser
#---

set(NAME parser)

add_library(${NAME})
target_sources(
  ${NAME}
  PUBLIC ${ANTLR_Listener_CXX_OUTPUTS} ${ANTLR_SampleGrammarLexer_CXX_OUTPUTS}
         ${ANTLR_SampleGrammarParser_CXX_OUTPUTS})

target_include_directories(${NAME}
                           PUBLIC ${ANTLR_SampleGrammarLexer_OUTPUT_DIR})
target_include_directories(${NAME}
                           PUBLIC ${ANTLR_SampleGrammarParser_OUTPUT_DIR})

# target_include_directories(${NAME} PUBLIC ${ANTLR_Listener_OUTPUT_DIR})

target_link_libraries(${NAME} antlr4_static)
