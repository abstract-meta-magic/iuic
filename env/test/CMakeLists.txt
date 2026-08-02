add_executable(iuic_env_persist_object_test
	"iuic-env-persist-object.test.cpp"
)

target_link_libraries(iuic_env_persist_object_test
	PRIVATE
	iuic_test
	iuic_env
)

add_test(NAME "iuic::env::persist[object_storage]" 
	COMMAND iuic_env_persist_object_test
)
