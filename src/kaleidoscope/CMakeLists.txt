# add_executable(kaleidoscope main.cpp)
add_executable(kaleidoscope kaleidoscope.cpp)

find_package(LLVM REQUIRED CONFIG)

add_llvm_libs(
  EXEC
  kaleidoscope
  LIBS
  core
  orcjit
  native)

target_link_libraries(
  kaleidoscope
  PRIVATE CLI11::CLI11
          fmt::fmt
          spdlog::spdlog
          ${llvm_libs}
          # myproject::myproject_options
          # myproject::myproject_warnings
)

target_include_directories(
  kaleidoscope PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
