add_library(${PROJECT_NAME}_underlying__tree STATIC)

target_sources(${PROJECT_NAME}_underlying__tree PUBLIC
	FILE_SET iuic_tree_modules
	TYPE CXX_MODULES
	FILES
	"iuic-tree.cppm"
)

target_link_libraries(${PROJECT_NAME}_underlying__tree
	PUBLIC ${PROJECT_NAME}_underlying__utils
	PUBLIC ${PROJECT_NAME}_underlying__erasure
	PUBLIC ${PROJECT_NAME}_underlying__tree_orderable
)

add_subdirectory(orderable)



