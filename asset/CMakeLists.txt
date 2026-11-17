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

target_link_libraries(${PROJECT_NAME}_asset
	PRIVATE ${PROJECT_NAME}_default
	PUBLIC ${PROJECT_NAME}_underlying
)

target_compile_features(${PROJECT_NAME}_asset
	PRIVATE
	cxx_std_23
)

add_library(${PROJECT_NAME}::asset ALIAS ${PROJECT_NAME}_asset)


if(${IUIC_BUILD_TESTS})
	add_subdirectory(test)
endif()

