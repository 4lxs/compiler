include(GenerateExportHeader)

set(NAME pt)

add_library(
  ${NAME}
  block.cpp
  context.cpp
  expr.cpp
  module.cpp
  stmt.cpp
  path.cpp
  decl.cpp
  node.cpp
  sema/nameresolution.cpp)

add_library(myproject::${NAME} ALIAS ${NAME})

target_link_libraries(${NAME} PRIVATE libs)

target_include_directories(
  ${NAME} ${WARNING_GUARD}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

set_target_properties(
  ${NAME}
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(${NAME} EXPORT_FILE_NAME
                       ${PROJECT_BINARY_DIR}/include/myproject/pt_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(${NAME} PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif()
