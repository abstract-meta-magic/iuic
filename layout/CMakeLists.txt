add_library(${PROJECT_NAME}_layout)

target_sources(${PROJECT_NAME}_layout PUBLIC
	FILE_SET iuic_underlying_modules
	TYPE CXX_MODULES
	FILES
	"iuic-layout.cppm"
	"iuic-layout-utils.cppm"
)

if(TARGET iuic_std_module)
		target_link_libraries(${PROJECT_NAME}_layout iuic_std_module)
endif()

target_link_libraries(${PROJECT_NAME}_layout
	iuic::underlying
	iuic::text
	iuic::env
)

target_compile_features(${PROJECT_NAME}_layout
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::layout ALIAS ${PROJECT_NAME}_layout)
