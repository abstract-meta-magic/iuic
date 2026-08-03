add_executable(${PROJECT_NAME}_underlying__cenv_gen)

target_sources(${PROJECT_NAME}_underlying__cenv_gen
	PUBLIC 
	"iuic-cenv-gen.main.cpp"
)

if(TARGET iuic_std_module)
	target_link_libraries(${PROJECT_NAME}_underlying__cenv_gen PRIVATE iuic_std_module)
endif()

## impl
add_library(${PROJECT_NAME}_underlying__cenv)

target_sources(${PROJECT_NAME}_underlying__cenv PUBLIC
	FILE_SET iuic_cenv_modules
	TYPE CXX_MODULES
	FILES
	"iuic-cenv.cppm"
)

set(IUIC_CENV_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen/iuic/")
set(IUIC_CENV_INC_FILE "${IUIC_CENV_GEN_DIR}/__iuic_cenv_define.inc")
file(MAKE_DIRECTORY ${IUIC_CENV_GEN_DIR})

add_custom_command(
	OUTPUT ${IUIC_CENV_INC_FILE}
	COMMAND ${PROJECT_NAME}_underlying__cenv_gen
	${IUIC_CENV_FILE}
	${IUIC_CENV_INC_FILE}
	DEPENDS ${IUIC_CENV_FILE} ${PROJECT_NAME}_underlying__cenv_gen
	COMMENT "Gen iuic.cenv file"
	VERBATIM
)

add_custom_target(${PROJECT_NAME}_underlying__cenv_gen_dep DEPENDS ${IUIC_CENV_INC_FILE})

if(EXISTS ${IUIC_CENV_FILE})
	add_dependencies(${PROJECT_NAME}_underlying__cenv ${PROJECT_NAME}_underlying__cenv_gen_dep)
	target_include_directories(${PROJECT_NAME}_underlying__cenv PRIVATE ${IUIC_CENV_GEN_DIR})
endif()


if(TARGET iuic_std_module)
	target_link_libraries(${PROJECT_NAME}_underlying__cenv PRIVATE iuic_std_module)
endif()

target_compile_features(${PROJECT_NAME}_underlying__cenv
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)

target_compile_options(${PROJECT_NAME}_underlying__cenv
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)
