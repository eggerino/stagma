cmake_minimum_required(VERSION 3.10)

project(stagma)

add_executable(stagma
    "src/ast.c"
    "src/stagma.c"
)

add_executable(dynamic-array-test "src/dynamic_array.test.c")
