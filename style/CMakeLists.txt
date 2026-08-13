add_library(${PROJECT_NAME}_style)

target_sources(${PROJECT_NAME}_style PUBLIC
	FILE_SET iuic_style_modules
	TYPE CXX_MODULES
	FILES
	"iuic-style.cppm"
)

if(${IUIC_USER_STD})
target_link_libraries(${PROJECT_NAME}_style PRIVATE iuic::user_std)
endif()

target_link_libraries(${PROJECT_NAME}_style
	PUBLIC
	iuic::underlying
)

target_compile_features(${PROJECT_NAME}_style
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::style ALIAS ${PROJECT_NAME}_style)
