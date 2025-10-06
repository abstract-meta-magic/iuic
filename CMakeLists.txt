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
	"src/iuic-computing-ctx.cppm"
	"src/iuic-ctx.cppm"
	"src/iuic-pseudo-selector.cppm"
	"src/iuic-base-color.cppm"
)

target_sources(${PROJECT_NAME}_core PUBLIC
	"src/iuic-base.cpp"
	"src/iuic-ctx.cpp"
	"src/iuic-fct.cpp"
	"src/iuic-layout-utils.cpp"
	"src/iuic-computing-ctx.cpp"
)

## set hash

if(IUIC_HASH_TYPE STREQUAL "xxHash3")
if(DEFINED IUIC_HASH_INCLUDE_DIR)
message("iuic::core xxHash3 added")
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


add_subdirectory(test)
