function(add_valgrind_test TEST_NAME TEST_TARGET)
    find_program(VALGRIND "valgrind")
    if(NOT VALGRIND)
        return()
    endif()
    
    add_test(NAME ${TEST_NAME}
        COMMAND ${VALGRIND}
            --leak-check=full
            --error-exitcode=1
	    --track-origins=yes
	    -s
            $<TARGET_FILE:${TEST_TARGET}>
    )
    
    set_tests_properties(${TEST_NAME} PROPERTIES
        TIMEOUT 300
        LABELS "valgrind"
    )
endfunction()




add_executable(iuic_env_persist_object_test
	"iuic-env-persist-object.test.cpp"
)

target_link_libraries(iuic_env_persist_object_test
	PRIVATE
	iuic_test
	iuic_env
)

#add_test(NAME "iuic::env::persist[object_storage]" 
#	COMMAND iuic_env_persist_object_test
#)

add_valgrind_test("iuic::env::persist[object_storage]" iuic_env_persist_object_test)
