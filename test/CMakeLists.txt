cmake_minimum_required(VERSION 3.30)


find_package(SDL3 REQUIRED)

add_executable(te base.cpp)

target_link_libraries(te iuic SDL3::SDL3)
