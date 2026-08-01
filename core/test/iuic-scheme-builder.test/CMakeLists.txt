set(TEST_NAMES
    # uid
    uid_static
    uid_ordered
    uid_stable
    # mem
    memory_persist
    memory_tmp
    # style
    style_sid
    # state
    state
    # policy
    policy
    # text
    text
    # event ???
)

foreach(TEST_NAME ${TEST_NAMES})
    string(REPLACE "_" "-" SOURCE_FILE "${TEST_NAME}.cpp")
    
    add_executable(${TEST_NAME})
    target_link_libraries(${TEST_NAME} PRIVATE iuic_test iuic_core)
    target_sources(${TEST_NAME} PUBLIC ${SOURCE_FILE})
    
    string(REPLACE "_" "::" FULL_NAME "${TEST_NAME}")
    add_test(NAME "iuic-scheme-builder[${FULL_NAME}]" 
             COMMAND ${TEST_NAME} 
             LABELS "unit;core")
endforeach()
