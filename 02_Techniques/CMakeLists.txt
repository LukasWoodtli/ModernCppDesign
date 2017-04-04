cmake_minimum_required (VERSION 2.8.7)

add_executable(PartialTemplateSpecialization PartialTemplateSpecialization.cpp)
add_test(PartialTemplateSpecialization PartialTemplateSpecialization)

add_executable(Int2Type Int2Type.cpp)
add_test(Int2Type Int2Type)

add_executable(Type2Type Type2Type.cpp)
add_test(Type2Type Type2Type)

add_executable(Convertibility Convertibility.cpp)
add_test(Convertibility Convertibility)

add_executable(TypeInfoWrapper TypeInfoWrapper.cpp)
add_test(TypeInfoWrapper TypeInfoWrapper)
