add_library(${PROJECT_NAME}_underlying__utils STATIC)

target_sources(${PROJECT_NAME}_underlying__utils PUBLIC
	FILE_SET iuic_utils_modules
	TYPE CXX_MODULES
	FILES
	"iuic-utils-decl.cppm"
	"iuic-utils.cppm"
	"iuic-advance.cppm"
	"iuic-func.cppm"
)

target_sources(${PROJECT_NAME}_underlying__utils
	PUBLIC
	"iuic-advance.cpp"
)

if(${IUIC_USER_STD})
target_link_libraries(${PROJECT_NAME}_underlying__utils PRIVATE iuic::user_std)
endif()

target_link_libraries(${PROJECT_NAME}_underlying__utils PUBLIC
	${PROJECT_NAME}_underlying__cenv
	${PROJECT_NAME}_underlying__erasure
)


target_compile_features(${PROJECT_NAME}_underlying__utils
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)


target_compile_options(${PROJECT_NAME}_underlying__utils
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)

