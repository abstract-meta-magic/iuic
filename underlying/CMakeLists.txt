add_library(${PROJECT_NAME}_underlying STATIC)


add_subdirectory(cenv)
add_subdirectory(utils)
add_subdirectory(tree)
add_subdirectory(units)
add_subdirectory(erasure)
add_subdirectory(query)

target_sources(${PROJECT_NAME}_underlying PUBLIC
	FILE_SET iuic_underlying_modules
	TYPE CXX_MODULES
	FILES
	"iuic-underlying.cppm"
)

if(${IUIC_USER_STD})
	target_link_libraries(${PROJECT_NAME}_underlying PRIVATE iuic::user_std)
endif()

## UNDERLYING LIBS
target_link_libraries(${PROJECT_NAME}_underlying
	PUBLIC ${PROJECT_NAME}_underlying__cenv
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


target_compile_options(${PROJECT_NAME}_underlying
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)
