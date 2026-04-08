add_library(${PROJECT_NAME}_underlying STATIC)



target_sources(${PROJECT_NAME}_underlying PUBLIC
	FILE_SET iuic_underlying_modules
	TYPE CXX_MODULES
	FILES
	"iuic-underlying.cppm"
	"iuic-units.cppm"
	"iuic-utils.cppm"
	"iuic-advance.cppm"
	"iuic-test.cppm"
	"iuic-erasure.cppm"
	"iuic-external.cppm"
	"iuic-utils-tree.cppm"
	"iuic-utils-tree-node-type.cppm"
	"iuic-utils-tree-hierarchy-bfs.cppm"
	"iuic-utils-tree-hierarchy-dfs.cppm"
	"iuic-utils-tree-hierarchy-unordered.cppm"
	"iuic-utils-tree-flat-bfs-type.cppm"
	"iuic-utils-tree-flat-dfs-type.cppm"
	"iuic-utils-tree-flat-unordered-type.cppm"
)
target_sources(${PROJECT_NAME}_underlying PUBLIC
	"iuic-advance.cpp"
)

if(IUIC_TMP_CXX_STD)
		target_link_libraries(${PROJECT_NAME}_underlying PUBLIC tmp_cxx_std)
endif()

target_compile_features(${PROJECT_NAME}_underlying
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23)

add_library(${PROJECT_NAME}::underlying ALIAS ${PROJECT_NAME}_underlying)
