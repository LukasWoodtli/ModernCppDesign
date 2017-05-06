cmake_minimum_required (VERSION 2.8.7)

set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -std=c++11 -Wno-inaccessible-base")

add_executable(Typelists Typelists.cpp)
add_test(Typelists Typelists)


