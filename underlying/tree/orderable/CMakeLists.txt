add_library(${PROJECT_NAME}_underlying__tree_orderable STATIC)

target_sources(${PROJECT_NAME}_underlying__tree_orderable PUBLIC
	FILE_SET iuic_underlying__tree_orderable_fileset
	TYPE CXX_MODULES
	FILES
	"iuic-tree-hierarchy.cppm"
	"iuic-tree-impl.cppm"
	"iuic-tree-ranges.cppm"
	"iuic-tree-orders.cppm"
	"iuic-tree-orderable.cppm"
)

target_link_libraries(${PROJECT_NAME}_underlying__tree_orderable
	PRIVATE ${PROJECT_NAME}_default
	PUBLIC ${PROJECT_NAME}_underlying__utils
	PUBLIC ${PROJECT_NAME}_underlying__erasure
	PUBLIC ${PROJECT_NAME}_underlying__iterator
)

target_compile_features(${PROJECT_NAME}_underlying__tree_orderable
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)

if(${IUIC_BUILD_TESTS})
	add_subdirectory(test)
endif()
