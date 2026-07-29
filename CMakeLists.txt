cmake_minimum_required(VERSION 3.31)

project(iuic LANGUAGES CXX)

option(IUIC_BUILD_TESTS "Build unit tests" ON)
## project ENV ?

## lvl - 0
add_subdirectory(test)
add_subdirectory(underlying)
## lvl - 1
add_subdirectory(text)
add_subdirectory(symbol)
add_subdirectory(state)
add_subdirectory(style)
add_subdirectory(events)
## lvl - 3
add_subdirectory(env)
add_subdirectory(layout)
## Main
add_subdirectory(core)
