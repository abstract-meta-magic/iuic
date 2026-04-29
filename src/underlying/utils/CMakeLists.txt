add_library(${PROJECT_NAME}_underlying__utils STATIC)

target_sources(${PROJECT_NAME}_underlying__utils PUBLIC
	FILE_SET iuic_utils_modules
	TYPE CXX_MODULES
	FILES
	"iuic-erasure.cppm"
	"iuic-utils-decl.cppm"
	"iuic-external.cppm"
	"iuic-utils.cppm"
	"iuic-advance.cppm"
)

target_sources(${PROJECT_NAME}_underlying__utils
	PUBLIC
	"iuic-advance.cpp"
)


if(IUIC_TMP_CXX_STD)
		target_link_libraries(${PROJECT_NAME}_underlying__utils PUBLIC tmp_cxx_std)
endif()


target_compile_features(${PROJECT_NAME}_underlying
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)
