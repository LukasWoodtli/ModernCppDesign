cmake_minimum_required(VERSION 2.8.7)

project(ModernCppDesign)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_COLOR_MAKEFILE   ON)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-W -Wall -Wextra  -Werror -std=c++11")

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE DEBUG)
endif()

enable_testing()


add_subdirectory("01_Policy_Based_Class_Design")
add_subdirectory("02_Techniques")
add_subdirectory("03_Typelists")

