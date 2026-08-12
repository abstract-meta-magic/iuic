add_executable(packager
	"packager.test.cpp"
)

target_link_libraries(packager
	PRIVATE
	iuic_test
	iuic_events
)

add_test(NAME "iuic::event"
	COMMAND packager
)
