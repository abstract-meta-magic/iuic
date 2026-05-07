cmake_minimum_required(VERSION 3.31)

project(iuic LANGUAGES CXX)

add_subdirectory(src/underlying)
add_subdirectory(src/text)
add_subdirectory(src/state)

add_library(${PROJECT_NAME}_core STATIC)

target_link_libraries(${PROJECT_NAME}_core iuic::underlying iuic::text iuic::state)

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
	"src/iuic-style.cppm"
	"src/iuic-environment-tmp.cppm"
	"src/iuic-environment-persist.cppm"
	"src/iuic-token.cppm"
	"src/iuic-key-code.cppm"
	"src/iuic-event.cppm"
	"src/iuic-event-collector.cppm"
	"src/iuic-layout.def.cppm"
	"src/iuic-layout-utils.cppm"
	# "src/iuic-kernel.cppm" [depricated]
	"src/iuic-ctx.cppm"
	"src/iuic-machine-dispatcher.cppm"
	"src/iuic-base-color.cppm"
	"src/iuic-policy.cppm"
	"src/iuic-scheme-base.cppm"
	"src/iuic-scheme-compute.cppm"
	"src/iuic-scheme-explorer.cppm"
	"src/iuic-scheme-builder.cppm"
	# "src/iuic-scheme-hitscan.cppm" [depricated]
	"src/iuic-core.cppm"
)

target_sources(${PROJECT_NAME}_core PUBLIC
	"src/iuic-scheme-compute.cpp"
	"src/iuic-ctx.cpp"
	"src/iuic-layout-utils.cpp"
)

## Main alias
add_library(${PROJECT_NAME}::core ALIAS ${PROJECT_NAME}_core)
