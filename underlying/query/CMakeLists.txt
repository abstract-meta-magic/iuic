add_library(${PROJECT_NAME}_underlying__query STATIC)


target_sources(${PROJECT_NAME}_underlying__query PUBLIC
	FILE_SET iuic_underlying_query_modules
	TYPE CXX_MODULES
	FILES
	"iuic-query.cppm"
)

if(${IUIC_USER_STD})
target_link_libraries(${PROJECT_NAME}_underlying__query PRIVATE iuic::user_std)
endif()

target_link_libraries(${PROJECT_NAME}_underlying__query PRIVATE ${PROJECT_NAME}_underlying__erasure)

target_compile_features(${PROJECT_NAME}_underlying__query
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)


target_compile_options(${PROJECT_NAME}_underlying__query
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)
