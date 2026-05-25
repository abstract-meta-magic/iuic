add_library(${PROJECT_NAME}_underlying__utils STATIC)

target_sources(${PROJECT_NAME}_underlying__utils PUBLIC
	FILE_SET iuic_utils_modules
	TYPE CXX_MODULES
	FILES
	"iuic-utils-decl.cppm"
	"iuic-external.cppm"
	"iuic-utils.cppm"
	"iuic-advance.cppm"
	"iuic-cenv.cppm"
)

target_sources(${PROJECT_NAME}_underlying__utils
	PUBLIC
	"iuic-advance.cpp"
)


if(TARGET iuic_std_module)
		target_link_libraries(${PROJECT_NAME}_underlying__utils PUBLIC iuic_std_module)
endif()

target_link_libraries(${PROJECT_NAME}_underlying__utils PUBLIC ${PROJECT_NAME}_underlying__erasure)


target_compile_features(${PROJECT_NAME}_underlying
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)
