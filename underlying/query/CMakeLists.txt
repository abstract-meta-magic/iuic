add_library(${PROJECT_NAME}_underlying__query STATIC)

target_sources(${PROJECT_NAME}_underlying__query PUBLIC
	FILE_SET iuic_underlying_query_modules
	TYPE CXX_MODULES
	FILES
	"iuic-query.cppm"
)

target_link_libraries(${PROJECT_NAME}_underlying__query
	PRIVATE ${PROJECT_NAME}_default
	PRIVATE ${PROJECT_NAME}_underlying__erasure
)

target_compile_features(${PROJECT_NAME}_underlying__query
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)
