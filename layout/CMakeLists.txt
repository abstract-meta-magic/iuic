add_library(${PROJECT_NAME}_layout)

target_sources(${PROJECT_NAME}_layout PUBLIC
	FILE_SET iuic_underlying_modules
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

if(${IUIC_USER_STD})
target_link_libraries(${PROJECT_NAME}_layout PRIVATE iuic::user_std)
endif()


target_link_libraries(${PROJECT_NAME}_layout
	PUBLIC
	iuic::underlying
	iuic::text
	iuic::env
)

target_compile_features(${PROJECT_NAME}_layout
	PRIVATE
	cxx_std_23
)

target_compile_options(${PROJECT_NAME}_layout
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)

add_library(${PROJECT_NAME}::layout ALIAS ${PROJECT_NAME}_layout)


if(${IUIC_BUILD_TESTS})
	add_subdirectory(test)
endif()
