add_library(${PROJECT_NAME}_state STATIC)



target_sources(${PROJECT_NAME}_state PUBLIC
	FILE_SET iuic_state_modules
	TYPE CXX_MODULES
	FILES
	"iuic-state-decl.cppm"
	"iuic-state-value.cppm"
	"iuic-state.cppm"
)

if(IUIC_TMP_CXX_STD)
	target_link_libraries(${PROJECT_NAME}_state PUBLIC tmp_cxx_std)
endif()

target_compile_features(${PROJECT_NAME}_state
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23)

target_link_libraries(${PROJECT_NAME}_state PRIVATE iuic::underlying)

target_compile_options(${PROJECT_NAME}_state
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)


add_library(${PROJECT_NAME}::state ALIAS ${PROJECT_NAME}_state)
