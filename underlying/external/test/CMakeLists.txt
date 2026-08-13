
add_executable(binding_test
	"binding.test.cpp"
)

target_link_libraries(binding_test
	PRIVATE
	iuic_test
	iuic_underlying__external
)

add_test(NAME "iuic::external[binding]"
	COMMAND binding_test
)
