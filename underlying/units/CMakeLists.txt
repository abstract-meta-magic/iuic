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

if(TARGET iuic_std_module)
		target_link_libraries(${PROJECT_NAME}_underlying__units PUBLIC iuic_std_module)
endif()

target_link_libraries(${PROJECT_NAME}_underlying__units
	PUBLIC ${PROJECT_NAME}_underlying__utils
)

target_compile_features(${PROJECT_NAME}_underlying__units
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)

target_compile_options(${PROJECT_NAME}_underlying__units
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)
