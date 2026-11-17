add_library(${PROJECT_NAME}_underlying STATIC)

# 1
add_subdirectory(cenv)
add_subdirectory(erasure)
# 2
add_subdirectory(utils)
add_subdirectory(units)
add_subdirectory(query)
# 3
add_subdirectory(iterator)
add_subdirectory(tree)

target_sources(${PROJECT_NAME}_underlying PUBLIC
	FILE_SET iuic_underlying_modules
	TYPE CXX_MODULES
	FILES
	"iuic-underlying.cppm"
)

## UNDERLYING LIBS
target_link_libraries(${PROJECT_NAME}_underlying
	PRIVATE ${PROJECT_NAME}_default
	PUBLIC ${PROJECT_NAME}_underlying__cenv
	PUBLIC ${PROJECT_NAME}_underlying__erasure
	PUBLIC ${PROJECT_NAME}_underlying__utils
	PUBLIC ${PROJECT_NAME}_underlying__tree
	PUBLIC ${PROJECT_NAME}_underlying__units
	PUBLIC ${PROJECT_NAME}_underlying__query
)

target_compile_features(${PROJECT_NAME}_underlying
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)

add_library(${PROJECT_NAME}::underlying ALIAS ${PROJECT_NAME}_underlying)
