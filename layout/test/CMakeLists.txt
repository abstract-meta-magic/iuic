add_executable(test_layout)

target_sources(test_layout PRIVATE
	base.cpp
)

target_link_libraries(test_layout PUBLIC 
	PRIVATE ${PROJECT_NAME}_default
	PRIVATE ${PROJECT_NAME}_test
	PRIVATE ${PROJECT_NAME}_underlying
	PRIVATE ${PROJECT_NAME}_layout
)

add_test(NAME "iuic::layout"
	COMMAND  $<TARGET_FILE:test_layout>
	LABELS "unit;tree"
)
