add_library(${PROJECT_NAME}_layout)

target_sources(${PROJECT_NAME}_layout PUBLIC
	FILE_SET iuic_layout_modules
	TYPE CXX_MODULES
	FILES
	"iuic-layout-decl.cppm"
	"iuic-layout-interface.cppm"
	# utils
	"iuic-layout-utils-base.cppm"
	"iuic-layout-utils-frame-measure.cppm"
	"iuic-layout-utils-frame-arrange.cppm"
	"iuic-layout-utils-text-measure.cppm"
	"iuic-layout-utils-text-arrange.cppm"
	# final
	"iuic-layout-utils.cppm"
	"iuic-layout.cppm"
)

target_link_libraries(${PROJECT_NAME}_layout
	PRIVATE ${PROJECT_NAME}_default
	PUBLIC ${PROJECT_NAME}_underlying
	PUBLIC ${PROJECT_NAME}_text
	PUBLIC ${PROJECT_NAME}_env
)

target_compile_features(${PROJECT_NAME}_layout
	PRIVATE
	cxx_std_23
)

add_library(${PROJECT_NAME}::layout ALIAS ${PROJECT_NAME}_layout)


if(${IUIC_BUILD_TESTS})
	add_subdirectory(test)
endif()
