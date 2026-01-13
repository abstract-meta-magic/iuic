add_library(${PROJECT_NAME}_underlying STATIC)



target_sources(${PROJECT_NAME}_underlying PUBLIC
	FILE_SET iuic_underlying_modules
	TYPE CXX_MODULES
	FILES
	"iuic-underlying.cppm"
	"iuic-units.cppm"
	"iuic-erasure.cppm"
	"iuic-utils.cppm"
	"iuic-external.cppm"
)


if(IUIC_TMP_CXX_STD)
	target_link_libraries(${PROJECT_NAME}_underlying PUBLIC tmp_cxx_std)
endif()

target_compile_features(${PROJECT_NAME}_underlying
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::underlying ALIAS ${PROJECT_NAME}_underlying)
