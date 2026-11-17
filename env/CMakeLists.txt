add_library(${PROJECT_NAME}_env)

target_sources(${PROJECT_NAME}_env PUBLIC
	FILE_SET iuic_env_modules
	TYPE CXX_MODULES
	FILES
	"iuic-env-tmp-exception.cppm"
	"iuic-env-tmp-policy.cppm"
	"iuic-env-tmp-style.cppm"
	"iuic-env-tmp-memory.cppm"
	"iuic-env-tmp.cppm"
	"iuic-env-persist-object.cppm"
	"iuic-env-persist-state.cppm"
	"iuic-env-persist.cppm"
	"iuic-env-domain.cppm"
	"iuic-env.cppm"
)

target_link_libraries(${PROJECT_NAME}_env
	PRIVATE ${PROJECT_NAME}_default
	PUBLIC ${PROJECT_NAME}_underlying
	PUBLIC ${PROJECT_NAME}_state
	PUBLIC ${PROJECT_NAME}_style
	PUBLIC ${PROJECT_NAME}_text
	PUBLIC ${PROJECT_NAME}_events
)

target_compile_features(${PROJECT_NAME}_env
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::env ALIAS ${PROJECT_NAME}_env)

if(${IUIC_BUILD_TESTS})
  add_subdirectory(test)
endif()
