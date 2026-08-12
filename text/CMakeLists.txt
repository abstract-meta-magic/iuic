add_library(${PROJECT_NAME}_text)

target_sources(${PROJECT_NAME}_text PUBLIC
	FILE_SET iuic_text_modules
	TYPE CXX_MODULES
	FILES
	"iuic-text.cppm"
	"iuic-glyph.cppm"
	"iuic-atlas.cppm"
	"iuic-token.cppm"
)

target_sources(${PROJECT_NAME}_text PUBLIC
	"iuic-atlas.cpp"
)

if(${IUIC_USER_STD})
	target_link_libraries(${PROJECT_NAME}_text PRIVATE iuic::user_std)
endif()


target_link_libraries(${PROJECT_NAME}_text PUBLIC iuic::underlying)

target_compile_features(${PROJECT_NAME}_text
	PRIVATE
	cxx_std_23
)

target_compile_options(${PROJECT_NAME}_text PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)

add_library(${PROJECT_NAME}::text ALIAS ${PROJECT_NAME}_text)
