cmake_minimum_required(VERSION 3.30)

## wrong ?
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
## wrong ?
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(iuic LANGUAGES CXX)


option(IUIC_PRETTY "Включить pretty" OFF)
option(IUIC_INFO "Вывод информации" ON)

add_library(${PROJECT_NAME}_core STATIC)

if(IUIC_INFO)
message("iuic::core : static")
endif()
# Modules
target_sources(${PROJECT_NAME}_core PUBLIC
	FILE_SET iuic_core_modules 
	TYPE CXX_MODULES
	FILES
	"src/iuic-base.cppm"
	"src/iuic-hash.cppm"
	"src/iuic-fct.cppm"
	"src/iuic-storage.cppm"
	"src/iuic-animator.cppm"
	"src/iuic-key-code.cppm"
	"src/iuic-event.cppm"
	"src/iuic-render.cppm"
	"src/iuic-transform.cppm"
	"src/iuic-box-layout.cppm"
	"src/iuic-layout.cppm"
	"src/iuic-computing-ctx.cppm"
	"src/iuic-ctx.cppm"
	"src/iuic-pseudo-selector.cppm"
)

target_sources(${PROJECT_NAME}_core PUBLIC
	"src/iuic-base.cpp"
	"src/iuic-hash.cpp"
	"src/iuic-computing-ctx.cpp"
	"src/iuic-ctx.cpp"
	"src/iuic-fct.cpp"
	"src/iuic-layout-utils.cpp"
)

## Main alias
add_library(${PROJECT_NAME}::core ALIAS ${PROJECT_NAME}_core)


## WARNING - TMP SOLUTION
## xxHash - START
add_library(xxLib STATIC IMPORTED)
set_target_properties(xxLib PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/tmp_solutions/hash/libxxhash.a)

target_link_libraries(${PROJECT_NAME}_core xxLib)
## xxHash - END

if(IUIC_PRETTY)

## Простое сообщение
if(IUIC_INFO)
message("iuic::pretty : static")
endif()

## Позже будет доделана
add_library(${PROJECT_NAME}_pretty STATIC)

target_link_libraries(${PROJECT_NAME}_pretty PUBLIC ${PROJECT_NAME}_core)

target_sources(${PROJECT_NAME}_pretty PUBLIC
	FILE_SET iuic_pretty_modules 
	TYPE CXX_MODULES
	FILES
	"src/iuic-pretty.cppm" # other module
)

## pretty alias
add_library(${PROJECT_NAME}::pretty ALIAS ${PROJECT_NAME}_pretty)
endif()

## Tests
add_subdirectory(test)
