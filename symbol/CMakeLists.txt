add_library(${PROJECT_NAME}_symbol)

target_sources(${PROJECT_NAME}_symbol PUBLIC
	FILE_SET iuic_symbol_modules
	TYPE CXX_MODULES
	FILES
	"iuic-symbol-decl.cppm"
	"iuic-symbol-api-private.cppm"
	"iuic-symbol-api-public.cppm"
	"iuic-symbol.cppm"
)

if(TARGET iuic_std_module)
		target_link_libraries(${PROJECT_NAME}_symbol iuic_std_module)
endif()

target_link_libraries(${PROJECT_NAME}_symbol iuic::underlying)

target_compile_features(${PROJECT_NAME}_symbol
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::symbol ALIAS ${PROJECT_NAME}_symbol)
