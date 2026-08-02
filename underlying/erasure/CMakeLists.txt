add_library(${PROJECT_NAME}_underlying__erasure STATIC)


target_sources(${PROJECT_NAME}_underlying__erasure PUBLIC
	FILE_SET iuic_erasure_modules
	TYPE CXX_MODULES
	FILES
	"iuic-decl.cppm"
	"iuic-property.cppm"
	"iuic-type.cppm"
	# visited
	"iuic-visited-base.cppm"
	"iuic-visited-as-mutable.cppm"
	"iuic-visited-as-const.cppm"
	"iuic-visited-as-garbage.cppm"
	"iuic-visited-as-factory.cppm"
	#
	"iuic-visited.cppm"
	"iuic-erasure.cppm"
)

target_link_libraries(${PROJECT_NAME}_underlying__erasure PUBLIC ${PROJECT_NAME}_underlying__cenv)

if(TARGET iuic_std_module)
		target_link_libraries(${PROJECT_NAME}_underlying__erasure PUBLIC iuic_std_module)
endif()

target_compile_features(${PROJECT_NAME}_underlying__erasure
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)
