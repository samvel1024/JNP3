cmake_minimum_required(VERSION 3.10)
project(JNP3)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(ERR11)
add_executable(JNP3 err_runtime.cc wallet.h wallet.cc)
