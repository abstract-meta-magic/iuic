add_library(${PROJECT_NAME}_text)

target_sources(${PROJECT_NAME}_text PUBLIC
	FILE_SET iuic_text_modules
	TYPE CXX_MODULES
	FILES
	"iuic-text-base.cppm"
	"iuic-text-decoded.cppm"
	"iuic-text-decoded-tool.cppm"
	"iuic-text-present.cppm"
	"iuic-text-present-tool.cppm"
	"iuic-text-model.cppm"
	"iuic-text-model-profile.cppm"
	"iuic-text-model-decoder.cppm"
	"iuic-text-model-tool.cppm"
	"iuic-text-data.cppm"
	"iuic-text.cppm"
)


if(${IUIC_USER_STD})
	target_link_libraries(${PROJECT_NAME}_text PRIVATE iuic::user_std)
endif()


target_link_libraries(${PROJECT_NAME}_text 
	PUBLIC
	iuic::underlying
	iuic::asset
)

target_compile_features(${PROJECT_NAME}_text
	PRIVATE
	cxx_std_23
)

target_compile_options(${PROJECT_NAME}_text PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)

add_library(${PROJECT_NAME}::text ALIAS ${PROJECT_NAME}_text)


if(${IUIC_BUILD_TESTS})
	add_subdirectory(test)
endif()
