add_library(${PROJECT_NAME}_underlying__tree STATIC)

target_sources(${PROJECT_NAME}_underlying__tree PUBLIC
	FILE_SET iuic_tree_modules
	TYPE CXX_MODULES
	FILES
	"iuic-tree.cppm"
)

if(${IUIC_USER_STD})
target_link_libraries(${PROJECT_NAME}_underlying__tree PRIVATE iuic::user_std)
endif()


target_link_libraries(${PROJECT_NAME}_underlying__tree
	PUBLIC ${PROJECT_NAME}_underlying__utils
	PUBLIC ${PROJECT_NAME}_underlying__erasure
	PUBLIC ${PROJECT_NAME}_underlying__tree_orderable
)

target_compile_features(${PROJECT_NAME}_underlying__tree
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)

target_compile_options(${PROJECT_NAME}_underlying__tree
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)

add_subdirectory(orderable)



