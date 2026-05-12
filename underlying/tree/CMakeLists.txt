add_library(${PROJECT_NAME}_underlying__tree STATIC)

target_sources(${PROJECT_NAME}_underlying__tree PUBLIC
	FILE_SET iuic_tree_modules
	TYPE CXX_MODULES
	FILES
	"iuic-tree.cppm"
	"iuic-tree-decl.cppm"
	# hierarchy
	"iuic-hierarchy-bfs.cppm"
	"iuic-hierarchy-dfs.cppm"
	"iuic-hierarchy-unordered.cppm"
	# flat-type
	"iuic-flat-bfs-type.cppm"
	"iuic-flat-dfs-type.cppm"
	"iuic-flat-unordered-type.cppm"
	# shift-impl
	"shift/iuic-shift.cppm"
	"shift/iuic-shift-bfs.cppm"
	"shift/iuic-shift-dfs.cppm"
	# ranges-impl
	"ranges/iuic-ranges.cppm"
	"ranges/iuic-ranges-hierarchy-bfs.cppm"
	"ranges/iuic-ranges-hierarchy-dfs.cppm"
	"ranges/iuic-ranges-hierarchy-unordered.cppm"
	"ranges/iuic-ranges-type-bfs.cppm"
	"ranges/iuic-ranges-type-dfs.cppm"
	"ranges/iuic-ranges-type-unordered.cppm"
)

target_link_libraries(${PROJECT_NAME}_underlying__tree
	PUBLIC ${PROJECT_NAME}_underlying__utils
)

target_compile_features(${PROJECT_NAME}_underlying__tree
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)
