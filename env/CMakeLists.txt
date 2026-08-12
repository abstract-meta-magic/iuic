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
	"iuic-env.cppm"
)

if(${IUIC_USER_STD})
target_link_libraries(${PROJECT_NAME}_env PRIVATE iuic::user_std)
endif()


target_link_libraries(${PROJECT_NAME}_env
	PUBLIC
	iuic::underlying
	iuic::state
	iuic::style
	iuic::events
)

target_compile_features(${PROJECT_NAME}_env
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::env ALIAS ${PROJECT_NAME}_env)


if(${IUIC_BUILD_TESTS})
  add_subdirectory(test)
endif()

target_compile_options(${PROJECT_NAME}_env PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)
