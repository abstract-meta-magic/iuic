cmake_minimum_required(VERSION 3.15)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(iuic)


include_directories(include)

add_library(${PROJECT_NAME}
  "./src/component.cpp"
)
