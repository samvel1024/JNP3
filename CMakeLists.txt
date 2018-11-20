cmake_minimum_required(VERSION 3.10)
project(JNP3)

set(CMAKE_CXX_STANDARD 17)

add_executable(JNP3 wallet_example.cpp wallet.h wallet.cc)
