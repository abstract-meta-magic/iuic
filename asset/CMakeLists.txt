add_library(${PROJECT_NAME}_asset)

target_sources(${PROJECT_NAME}_asset PUBLIC
	FILE_SET iuic_asset_modules
	TYPE CXX_MODULES
	FILES
	"iuic-asset-base.cppm"
	"iuic-asset-bind.cppm"
	"iuic-asset-declaration.cppm"
	"iuic-asset-static-instance.cppm"
	"iuic-asset-definition.cppm"
	"iuic-asset-instance.cppm"
	"iuic-asset-map.cppm"
	"iuic-asset-free-list.cppm"
	"iuic-asset-catalog.cppm"
	"iuic-asset.cppm"
)

if(${IUIC_USER_STD})
	target_link_libraries(${PROJECT_NAME}_asset PRIVATE iuic::user_std)
endif()

target_link_libraries(${PROJECT_NAME}_asset PUBLIC iuic::underlying)

target_compile_features(${PROJECT_NAME}_asset
	PRIVATE
	cxx_std_23
)

target_compile_options(${PROJECT_NAME}_asset PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)

add_library(${PROJECT_NAME}::asset ALIAS ${PROJECT_NAME}_asset)


if(${IUIC_BUILD_TESTS})
	add_subdirectory(test)
endif()

