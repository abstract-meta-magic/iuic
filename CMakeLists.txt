cmake_minimum_required(VERSION 3.31)


## set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
##	d0edc3af-4c50-42ea-a356-e2862fe7a444)

## wrong ?
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
## wrong ?
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(iuic LANGUAGES CXX)

add_subdirectory(utils/cmake)

## set(CMAKE_CXX_MODULE_STD 1)

option(IUIC_PRETTY "Включить pretty" OFF)
option(IUIC_INFO "Вывод информации" ON)

add_library(${PROJECT_NAME}_core STATIC)

target_include_directories(iuic_core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tmp_solutions/std/std/)


target_compile_features(${PROJECT_NAME}_core
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23)

if(IUIC_INFO)
message("iuic::core : static")
endif()
# Modules
target_sources(${PROJECT_NAME}_core PUBLIC
	FILE_SET iuic_core_modules 
	TYPE CXX_MODULES
	FILES
	"src/iuic-base.cppm"
	"src/iuic-text-font.cppm"
	"src/iuic-style.cppm"
	"src/iuic-text-fontset.cppm"
	"src/iuic-hash.cppm"
	"src/iuic-fct.cppm"
	"src/iuic-storage.def.cppm"
	"src/iuic-object-storage.cppm"
	"src/iuic-token.cppm"
	"src/iuic-text-buff.cppm"
	"src/iuic-text-present.cppm"
	"src/iuic-text-storage.cppm"
	"src/iuic-key-code.cppm"
	"src/iuic-event.cppm"
	"src/iuic-box-layout.cppm"
	"src/iuic-text-box-layout.cppm"
	"src/iuic-layout.def.cppm"
	"src/iuic-layout.cppm"
	"src/iuic-kernel.cppm"
	"src/iuic-ctx.cppm"
	"src/iuic-state.cppm"
	"src/iuic-state-transition.cppm"
	"src/iuic-base-color.cppm"
	"src/iuic-policy.cppm"
	"src/iuic-scheme.cppm"
	"src/iuic-scheme-builder.cppm"
	"tmp_solutions/std/std.cppm"
)

target_sources(${PROJECT_NAME}_core PUBLIC
	"src/iuic-base.cpp"
	"src/iuic-ctx.cpp"
	"src/iuic-fct.cpp"
	"src/iuic-text-present.cpp"
	"src/iuic-layout-utils.cpp"
	"src/iuic-computing-ctx.cpp"
	"src/iuic-state.cpp"
	"src/iuic-state-transition.cpp"
	"src/iuic-text-font.cpp"
	"src/iuic-kernel-impl.cpp"
)
## set hash

if(IUIC_HASH_TYPE STREQUAL "xxHash3")
if(DEFINED IUIC_HASH_INCLUDE_DIR)
message("iuic::core : xxHash3 added")
message("Hash dir "${IUIC_HASH_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME}_core
	PRIVATE ${IUIC_HASH_INCLUDE_DIR})
target_sources(${PROJECT_NAME}_core 
	PRIVATE "src/iuic-hash.xxhash.cpp")
else()
message(SEND_ERROR "iuic::core : not set IUIC_HASH_INCLUDE_DIR")
endif()

elseif(IUIC_HASH_TYPE STREQUAL "CityHash")
if(IUIC_HASH_INCLUDE_DIR)
target_include_directories(${PROJECT_NAME}_core ${IUIC_HASH_INCLUDE_DIR})
target_sources(${PROJECT_NAME}_core 
	PRIVATE "src/iuic-hash.cityhash.cpp")
else()
message(SEND_ERROR "iuic::core : not set IUIC_HASH_INCLUDE_DIR")
endif()
endif()

## Main alias
add_library(${PROJECT_NAME}::core ALIAS ${PROJECT_NAME}_core)

## Ppetty module
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


iuic_env_add_target(iuic_core)


iuic_env_add("iuic::tmp-buffsize" "512KB" NUM)
iuic_env_add("iuic::persist-buffsize" "1MB" NUM)
iuic_env_add("iuic::intro" "Hello World" STR)


iuic_env_targets()
iuic_env_vars()

#add_subdirectory(test)
#add_subdirectory(kit)
