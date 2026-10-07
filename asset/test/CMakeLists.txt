add_executable(test_map)

target_sources(test_map
	PRIVATE
	"map.cpp"
)

target_link_libraries(test_map
	PRIVATE
	iuic_test
	iuic::asset
)


add_test(NAME "iuic::asset::map"
	COMMAND test_map 
	LABELS "unit;asset"
)


add_executable(test_list)

target_sources(test_list
	PRIVATE
	"list.cpp"
)

target_link_libraries(test_list
	PRIVATE
	iuic_test
	iuic::asset
)


add_test(NAME "iuic::asset::list"
	COMMAND test_list
	LABELS "unit;asset"
)


add_executable(test_catalog)

target_sources(test_catalog
	PRIVATE
	"catalog.cpp"
)

target_link_libraries(test_catalog
	PRIVATE
	iuic_test
	iuic::asset
)


add_test(NAME "iuic::asset::catalog"
	COMMAND test_catalog
	LABELS "unit;asset"
)

