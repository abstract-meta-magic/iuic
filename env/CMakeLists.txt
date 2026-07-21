add_library(${PROJECT_NAME}_env)

target_sources(${PROJECT_NAME}_env PUBLIC
	FILE_SET iuic_env_modules
	TYPE CXX_MODULES
	FILES
	"iuic-env-tmp.cppm"
	"iuic-env-persist-decl.cppm"
	"iuic-env-persist-object.cppm"
	"iuic-env-persist-state.cppm"
	"iuic-env-persist.cppm"
	"iuic-env.cppm"
)


if(TARGET iuic_std_module)
		target_link_libraries(${PROJECT_NAME}_env iuic_std_module)
endif()

target_link_libraries(${PROJECT_NAME}_env
	iuic::underlying
	iuic::state
	iuic::style
	iuic::events
)

target_compile_features(${PROJECT_NAME}_env
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::env ALIAS ${PROJECT_NAME}_env)

# target_compile_options(${PROJECT_NAME}_env PRIVATE -O2 -g)
