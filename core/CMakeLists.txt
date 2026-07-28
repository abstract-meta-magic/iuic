cmake_minimum_required(VERSION 3.31)

add_library(${PROJECT_NAME}_core STATIC)

target_link_libraries(${PROJECT_NAME}_core
	iuic::underlying
	iuic::text
	iuic::symbol
	iuic::state
	iuic::style
	iuic::env
	iuic::layout
)

if(TARGET iuic_std_module)
target_link_libraries(${PROJECT_NAME}_core iuic_std_module)
endif()

target_compile_features(${PROJECT_NAME}_core
	PRIVATE cxx_std_23
)

# Modules
target_sources(${PROJECT_NAME}_core PUBLIC
	FILE_SET iuic_core_modules 
	TYPE CXX_MODULES
	FILES
	"iuic-ctx.cppm"
	"iuic-policy.cppm"
	"iuic-scheme-base.cppm"
	"iuic-scheme-compute.cppm"
	"iuic-scheme-query.cppm"
	"iuic-scheme-shared.cppm"
	"iuic-scheme-explorer.cppm"
	"iuic-scheme-builder.cppm"
	"iuic-core.cppm"
	PRIVATE
	"iuic-scheme-compute.cpp"
)

## Main alias
add_library(${PROJECT_NAME}::core ALIAS ${PROJECT_NAME}_core)

## IF TEST
if(IUIC_BUILD_TESTS)
  if(TARGET Catch2::Catch2)
    add_subdirectory(test)
  else()
    message(FATAL_ERROR "IUIC use BUILD_TESTS, but Catch2 not found")
  endif()
endif()

# target_compile_options(${PROJECT_NAME}_core PRIVATE -O2)
