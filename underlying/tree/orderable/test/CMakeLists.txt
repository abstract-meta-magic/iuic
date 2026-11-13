


add_executable(test_tree_orderable)


target_sources(test_tree_orderable PUBLIC
	"base.cpp"
)

target_link_libraries(test_tree_orderable PUBLIC
	iuic_test
	iuic_underlying__tree_orderable
)

target_compile_options(test_tree_orderable PRIVATE
	-g
	-O0
)

add_test(NAME "iuic::tree[orderable]"
	COMMAND  $<TARGET_FILE:test_tree_orderable>
	LABELS "unit;tree"
)
