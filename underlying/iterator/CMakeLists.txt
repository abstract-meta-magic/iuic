add_library(${PROJECT_NAME}_underlying__iterator STATIC)

target_sources(${PROJECT_NAME}_underlying__iterator PUBLIC
	FILE_SET iuic_iterator_modules
	TYPE CXX_MODULES
	FILES
	"iuic-iterator-base.cppm"
	"iuic-iterator-underlying-table.cppm"
	"iuic-sentinel.cppm"
	"iuic-iterator-basic.cppm"
	"iuic-iterator-access.cppm"
	"iuic-iterator-const-access.cppm"
	"iuic-iterator-move.cppm"
	"iuic-iterator-copy.cppm"
	"iuic-iterator-insert.cppm"
	"iuic-range.cppm"
	"iuic-iterator.cppm"
)

target_link_libraries(${PROJECT_NAME}_underlying__iterator 
	PRIVATE ${PROJECT_NAME}_default
	PUBLIC ${PROJECT_NAME}_underlying__cenv
	PUBLIC ${PROJECT_NAME}_underlying__erasure
)


target_compile_features(${PROJECT_NAME}_underlying__iterator
	PRIVATE cxx_std_20
	PRIVATE cxx_std_23
)
