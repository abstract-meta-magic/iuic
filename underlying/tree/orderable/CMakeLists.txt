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

if(${IUIC_USER_STD})
target_link_libraries(${PROJECT_NAME}_underlying__tree_orderable PRIVATE iuic::user_std)
endif()


target_link_libraries(${PROJECT_NAME}_underlying__tree_orderable
	PUBLIC ${PROJECT_NAME}_underlying__utils
	PUBLIC ${PROJECT_NAME}_underlying__erasure
)

target_compile_features(${PROJECT_NAME}_underlying__tree_orderable
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)

target_compile_options(${PROJECT_NAME}_underlying__tree_orderable
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)

target_compile_options(${PROJECT_NAME}_underlying__tree_orderable
	PRIVATE
	-g
	-O0
)

if(${IUIC_BUILD_TESTS})
	add_subdirectory(test)
endif()
