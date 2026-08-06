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
        LABELS "valgrind;unit;memory;env"
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

add_valgrind_test("iuic::env::persist[object_storage]" iuic_env_persist_object_test)

add_executable(iuic_env_persist_state_test
	"iuic-env-persist-state.test.cpp"
)

target_link_libraries(iuic_env_persist_state_test
	PRIVATE
	iuic_test
	iuic_env
)

add_valgrind_test("iuic::env::persist[state]" iuic_env_persist_state_test)

add_executable(iuic_env_tmp_style_test
	"iuic-env-tmp-style.test.cpp"
)

target_link_libraries(iuic_env_tmp_style_test
	PRIVATE
	iuic_test
	iuic_env
)

add_test(NAME "iuic::env::tmp[style]"
	COMMAND iuic_env_tmp_style_test
	LABELS "unit;env"
)


add_executable(iuic_env_tmp_policy_test
	"iuic-env-tmp-policy.test.cpp"
)

target_link_libraries(iuic_env_tmp_policy_test
	PRIVATE
	iuic_test
	iuic_env
)

add_test(NAME "iuic::env::tmp[policy]"
	COMMAND iuic_env_tmp_policy_test
	LABELS "unit;env"
)


add_executable(iuic_env_tmp_memory_test
	"iuic-env-tmp-memory.test.cpp"
)

target_link_libraries(iuic_env_tmp_memory_test
	PRIVATE
	iuic_test
	iuic_env
)

add_test(NAME "iuic::env::tmp[memory]"
	COMMAND iuic_env_tmp_memory_test
	LABELS "unit;env"
)
