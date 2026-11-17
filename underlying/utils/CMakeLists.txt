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

target_link_libraries(${PROJECT_NAME}_underlying__utils
	PRIVATE ${PROJECT_NAME}_default
	PUBLIC ${PROJECT_NAME}_underlying__cenv
	PUBLIC ${PROJECT_NAME}_underlying__erasure
)


target_compile_features(${PROJECT_NAME}_underlying__utils
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)

