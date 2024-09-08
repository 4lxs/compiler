#---
# Antlr
#---
message(STATUS "adding antlr ${CMAKE_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/antlr)
add_definitions(-DANTLR4CPP_STATIC)

include(ExternalAntlr4Cpp)
find_package(ANTLR REQUIRED)

# antlr_target(
#   Listener
#   X.g4
#   LISTENER
#   PACKAGE
#   parser)

antlr_target(
  Visitor
  X.g4
  VISITOR
  PACKAGE
  parser)

#---
# Parser
#---

set(NAME parser)

add_library(${NAME})
target_sources(
  ${NAME} PUBLIC ${ANTLR_Listener_CXX_OUTPUTS} ${ANTLR_Visitor_CXX_OUTPUTS}
                 ${ANTLR_XLexer_CXX_OUTPUTS})

target_include_directories(
  ${NAME}
  PUBLIC ${ANTLR4_INCLUDE_DIRS}
         ${ANTLR_XLexer_OUTPUT_DIR}
         ${ANTLR_Listener_OUTPUT_DIR}
         ${ANTLR_Visitor_OUTPUT_DIR})

target_link_libraries(${NAME} antlr4_static)
