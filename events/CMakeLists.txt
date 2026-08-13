add_library(${PROJECT_NAME}_events)

target_sources(${PROJECT_NAME}_events PUBLIC
	FILE_SET iuic_events_modules
	TYPE CXX_MODULES
	FILES
	"iuic-event-decl.cppm"
	"iuic-event-pool.cppm"
	"iuic-event-allocator.cppm"
	"iuic-event-packager.cppm"
	"iuic-event-dispatcher.cppm"
	"iuic-event-query.cppm"
	"iuic-event-factory.cppm"
	"iuic-event-hub.cppm"
	"iuic-event.cppm"
)

if(${IUIC_USER_STD})
target_link_libraries(${PROJECT_NAME}_events PRIVATE iuic::user_std)
endif()

target_link_libraries(${PROJECT_NAME}_events
	PUBLIC
	iuic::underlying
)

target_compile_features(${PROJECT_NAME}_events
	PRIVATE cxx_std_23)

target_compile_options(${PROJECT_NAME}_events
	PRIVATE
	${IUIC_DEFAULT_COMPILE_OPTIONS}
)

add_library(${PROJECT_NAME}::events ALIAS ${PROJECT_NAME}_events)


if(${IUIC_BUILD_TESTS})
add_subdirectory(test)
endif()
