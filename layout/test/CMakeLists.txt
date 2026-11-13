add_executable(test_layout)

target_sources(test_layout PRIVATE
	base.cpp
)

target_link_libraries(test_layout PUBLIC 
	iuic_test
	iuic_layout
)

target_compile_options(test_layout PRIVATE
	-g
	-O0
)

add_test(NAME "iuic::layout"
	COMMAND  $<TARGET_FILE:test_layout>
	LABELS "unit;tree"
)
