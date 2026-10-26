add_executable(test_model)


target_sources(test_model PRIVATE
	"model.cpp"
)

target_link_libraries(test_model 
	PRIVATE
	iuic_test
	iuic::text
)

add_test(NAME "iuic::text[model]"
	COMMAND $<TARGET_FILE:test_model>
	LABELS "unit;text"
)


add_executable(test_decoded)


target_sources(test_decoded PRIVATE
	"decoded.cpp"
)

target_link_libraries(test_decoded 
	PRIVATE
	iuic_test
	iuic::text
)

add_test(NAME "iuic::text[decoded]"
	COMMAND $<TARGET_FILE:test_decoded>
	LABELS "unit;text"
)



add_executable(test_present)


target_sources(test_present PRIVATE
	"present.cpp"
)

target_link_libraries(test_present 
	PRIVATE
	iuic_test
	iuic::text
)

add_test(NAME "iuic::text[present]"
	COMMAND $<TARGET_FILE:test_present>
	LABELS "unit;text"
)

