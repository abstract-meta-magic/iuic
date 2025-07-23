cmake_minimum_required(VERSION 3.30)



set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(iuic)

add_library(${PROJECT_NAME})

# Modules
target_sources(${PROJECT_NAME} PUBLIC
	FILE_SET iuic_modules 
	TYPE CXX_MODULES
	FILES
	"src/iuic-base.cppm"
	"src/iuic-ftc.cppm"
	"src/iuic-storage.cppm"
	"src/iuic-animator.cppm"
	"src/iuic-event.cppm"
	"src/iuic-ctx.cppm"
)

target_sources(${PROJECT_NAME} PUBLIC
	"src/iuic-ctx.cpp"
	"src/iuic-ftc.cpp"
)


# TODO : tests
add_subdirectory(test)
