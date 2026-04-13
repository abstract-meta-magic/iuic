




add_library(${PROJECT_NAME}_text)


target_sources(${PROJECT_NAME}_text PUBLIC
	FILE_SET iuic_underlying_modules
	TYPE CXX_MODULES
	FILES
	"iuic-text.cppm"
	"iuic-glyph.cppm"
	"iuic-atlas.cppm"
	"iuic-token.cppm"
)

if(IUIC_TMP_CXX_STD)
		target_link_libraries(${PROJECT_NAME}_underlying PUBLIC tmp_cxx_std)
endif()

target_link_libraries(${PROJECT_NAME}_text iuic::underlying)

target_compile_features(${PROJECT_NAME}_text
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::text ALIAS ${PROJECT_NAME}_text)
