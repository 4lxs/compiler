set(NAME antlr_test)

add_executable(${NAME} main.cpp)

target_link_libraries(${NAME} parser)
