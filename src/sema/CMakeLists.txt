include(GenerateExportHeader)

set(NAME sema)

add_library(${NAME} sema.cpp)

add_library(myproject::${NAME} ALIAS ${NAME})

target_link_libraries(
  ${NAME} PRIVATE fmt::fmt spdlog::spdlog myproject_options
                  # myproject_warnings
)

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
