cmake_minimum_required(VERSION 4.0)

add_library(${PROJECT_NAME}_core STATIC)



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

target_link_libraries(${PROJECT_NAME}_core
	PRIVATE ${PROJECT_NAME}_default
	PUBLIC ${PROJECT_NAME}_profile
	PUBLIC ${PROJECT_NAME}_underlying
	PUBLIC ${PROJECT_NAME}_text
	PUBLIC ${PROJECT_NAME}_state
	PUBLIC ${PROJECT_NAME}_style
	PUBLIC ${PROJECT_NAME}_env
	PUBLIC ${PROJECT_NAME}_layout
)

target_compile_features(${PROJECT_NAME}_core
	PRIVATE cxx_std_23
)

## Main alias
add_library(${PROJECT_NAME}::core ALIAS ${PROJECT_NAME}_core)

if(${IUIC_BUILD_TESTS})
	add_subdirectory(test)
endif()
