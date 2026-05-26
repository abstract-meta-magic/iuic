cmake_minimum_required(VERSION 3.31)

project(iuic LANGUAGES CXX)

## lvl - 0
add_subdirectory(underlying)
## lvl - 1
add_subdirectory(text)
add_subdirectory(state)
add_subdirectory(style)
## lvl - 3
add_subdirectory(events)
## lvl - 4
add_subdirectory(env)
add_subdirectory(layout)

add_library(${PROJECT_NAME}_core STATIC)

target_link_libraries(${PROJECT_NAME}_core
	iuic::underlying
	iuic::text
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
	"core/iuic-ctx.cppm"
	"core/iuic-policy.cppm"
	"core/iuic-proto-base.cppm"
	"core/iuic-scheme-base.cppm"
	"core/iuic-scheme-compute.cppm"
	"core/iuic-scheme-explorer.cppm"
	"core/iuic-scheme-builder.cppm"
	"core/iuic-core.cppm"
	PRIVATE
	"core/iuic-scheme-compute.cpp"
)

## Main alias
add_library(${PROJECT_NAME}::core ALIAS ${PROJECT_NAME}_core)
