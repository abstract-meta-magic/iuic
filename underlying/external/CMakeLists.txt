add_library(${PROJECT_NAME}_underlying__external STATIC)

target_sources(${PROJECT_NAME}_underlying__external PUBLIC
	FILE_SET iuic_utils_modules
	TYPE CXX_MODULES
	FILES
	"iuic-external-type.cppm" 
	"iuic-external-uri.cppm" 
	"iuic-external-block-base.cppm" 
	"iuic-external-block-static.cppm" 
	"iuic-external-block-dynamic.cppm" 
	"iuic-external-instance.cppm" 
	"iuic-external-binding.cppm" 
	"iuic-external.cppm" 
)

if(${IUIC_USER_STD})
target_link_libraries(${PROJECT_NAME}_underlying__external PRIVATE iuic::user_std)
endif()

target_link_libraries(${PROJECT_NAME}_underlying__external PUBLIC
	${PROJECT_NAME}_underlying__erasure
)


target_compile_features(${PROJECT_NAME}_underlying__external
	PRIVATE cxx_std_23
)


target_compile_options(${PROJECT_NAME}_underlying__external
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)


if(${IUIC_BUILD_TESTS})
	add_subdirectory(test)
endif()
