add_library(${PROJECT_NAME}_text)

target_sources(${PROJECT_NAME}_text PUBLIC
	FILE_SET iuic_underlying_modules
	TYPE CXX_MODULES
	FILES
	"iuic-text.cppm"
	"iuic-glyph.cppm"
	"iuic-atlas.cppm"
	"iuic-token.cppm"
	"iuic-i18n.cppm"
)

target_sources(${PROJECT_NAME}_text PUBLIC
	"iuic-atlas.cpp"
)

if(TARGET iuic_std_module)
		target_link_libraries(${PROJECT_NAME}_underlying PUBLIC iuic_std_module)
endif()

target_link_libraries(${PROJECT_NAME}_text iuic::underlying)

target_compile_features(${PROJECT_NAME}_text
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::text ALIAS ${PROJECT_NAME}_text)
