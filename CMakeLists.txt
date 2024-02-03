cmake_minimum_required(VERSION 3.10)

project(stagma)

add_executable(stagma
    "src/ast.c"
    "src/interpreter.c"
    "src/parser.c"
    "src/repl.c"
    "src/runtime.c"
    "src/stagma.c"
)
target_link_libraries(stagma m) # Link math.h

add_executable(dynamic-array-test "src/dynamic_array.test.c")
