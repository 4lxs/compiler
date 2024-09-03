set(NAME compiler)

add_executable(
  ${NAME}
  main.cpp
  generator.cpp
  generator.hpp
  main.cpp
  parser.cpp
  parser.hpp
  scanner.cpp
  scanner.hpp
  types.cpp
  types.hpp)

add_subdirectory(ast)

find_package(LLVM REQUIRED CONFIG)

add_llvm_libs(
  EXEC
  ${NAME}
  LIBS
  core
  orcjit
  native)

target_link_libraries(
  ${NAME}
  PRIVATE CLI11::CLI11
          fmt::fmt
          spdlog::spdlog
          ${llvm_libs}
          myproject::myproject_options
          myproject::myproject_warnings)

target_include_directories(
  ${NAME} PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
