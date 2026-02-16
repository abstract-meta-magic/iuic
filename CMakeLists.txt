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

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tmp_solutions)
	add_subdirectory(tmp_solutions/std)
endif()

add_subdirectory(utils/cmake)
add_subdirectory(src/underlying)
add_subdirectory(src/state)


option(IUIC_INFO "Вывод информации" ON)

add_library(${PROJECT_NAME}_core STATIC)

target_link_libraries(${PROJECT_NAME}_core iuic::underlying iuic::state)

if(IUIC_TMP_CXX_STD)
	target_link_libraries(${PROJECT_NAME}_core tmp_cxx_std)
endif()

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
	"src/iuic-style.cppm"
	"src/iuic-environment-tmp.cppm"
	"src/iuic-environment-persist.cppm"
	"src/iuic-token.cppm"
	"src/iuic-text-font.cppm"
	"src/iuic-text-fontset.cppm"
	"src/iuic-text-present.cppm"
	"src/iuic-text-buff.cppm"
	"src/iuic-key-code.cppm"
	"src/iuic-event.cppm"
	#"src/iuic-box-layout.cppm"
	#"src/iuic-text-box-layout.cppm"
	"src/iuic-layout.def.cppm"
	"src/iuic-layout-utils.cppm"
	# "src/iuic-kernel.cppm" [depricated]
	"src/iuic-ctx.cppm"
	"src/iuic-machine-dispatcher.cppm"
	"src/iuic-base-color.cppm"
	"src/iuic-policy.cppm"
	"src/iuic-scheme.cppm"
	"src/iuic-scheme-hitscan.cppm"
	"src/iuic-scheme-builder.cppm"
)

target_sources(${PROJECT_NAME}_core PUBLIC
	"src/iuic-ctx.cpp"
	"src/iuic-text-present.cpp"
	"src/iuic-layout-utils.cpp"
	"src/iuic-text-font.cpp"
	#"src/iuic-kernel-impl.cpp" [depricated]
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_library(backtrace__ SHARED IMPORTED)

set_target_properties(backtrace__ PROPERTIES
	IMPORTED_LOCATION ${BACKTRACE}/lib/libbacktrace.so
	INTERFACE_INCLUDE_DIRECTORIES ${BACKTRACE}/include
)

target_link_libraries(${PROJECT_NAME}_core backtrace__)


target_sources(${PROJECT_NAME}_core PUBLIC
	"src/test/iuic-state.test.cpp"
)

endif()

## Main alias
add_library(${PROJECT_NAME}::core ALIAS ${PROJECT_NAME}_core)



## PROTO START
iuic_env_add_target(iuic_core)


iuic_env_add("iuic::tmp-buffsize" "512KB" NUM)
iuic_env_add("iuic::persist-buffsize" "1MB" NUM)
iuic_env_add("iuic::intro" "Hello World" STR)


iuic_env_targets()
iuic_env_vars()
## PROTO END

add_subdirectory(test)
add_subdirectory(kit)
