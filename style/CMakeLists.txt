add_library(${PROJECT_NAME}_style)

target_sources(${PROJECT_NAME}_style PUBLIC
	FILE_SET iuic_style_modules
	TYPE CXX_MODULES
	FILES
	"iuic-style.cppm"
)

if(TARGET iuic_std_module)
		target_link_libraries(${PROJECT_NAME}_style iuic_std_module)
endif()

target_link_libraries(${PROJECT_NAME}_style iuic::underlying)

target_compile_features(${PROJECT_NAME}_style
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::style ALIAS ${PROJECT_NAME}_style)
