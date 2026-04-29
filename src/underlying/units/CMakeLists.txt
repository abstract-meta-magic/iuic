add_library(${PROJECT_NAME}_underlying__units STATIC)


target_sources(${PROJECT_NAME}_underlying__units PUBLIC
	FILE_SET iuic_underlying_modules
	TYPE CXX_MODULES
	FILES
	"iuic-units.cppm"
	"iuic-units-decl.cppm"
	"iuic-units-op.cppm"
	"iuic-units-ui.cppm"
)

if(IUIC_TMP_CXX_STD)
		target_link_libraries(${PROJECT_NAME}_underlying__units PUBLIC tmp_cxx_std)
endif()

target_link_libraries(${PROJECT_NAME}_underlying__units
	PUBLIC ${PROJECT_NAME}_underlying__utils
)

target_compile_features(${PROJECT_NAME}_underlying__units
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)
