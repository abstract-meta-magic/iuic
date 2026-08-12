add_executable(base_test
	base.cpp
)

target_link_libraries(base_test PRIVATE
	iuic_test
	iuic_layout
)

add_test(NAME "iuic::layout[base]"
	COMMAND base_test
)
