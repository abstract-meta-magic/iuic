
add_executable(binding_test
	"binding.test.cpp"
)

target_link_libraries(binding_test
	PRIVATE
	iuic_test
	iuic_underlying__external
)

find_program(VALGRIND valgrind)

## TODO : ERROR MSG

add_test(NAME "iuic::external[binding]"
	COMMAND ${VALGRIND} --error-exitcode=1 --track-origins=yes --leak-check=full ./binding_test
)
