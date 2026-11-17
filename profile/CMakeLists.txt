add_library(${PROJECT_NAME}_profile)

target_sources(${PROJECT_NAME}_profile PUBLIC
	FILE_SET iuic_profile_modules
	TYPE CXX_MODULES
	FILES
	$<IF:$<BOOL:${IUIC_USE_PROFILER}>,iuic-profile-tracy.cppm,iuic-profile-none.cppm>
	"iuic-profile.cppm"
)

target_link_libraries(${PROJECT_NAME}_profile
	PRIVATE ${PROJECT_NAME}_default
	PRIVATE $<IF:$<BOOL:${IUIC_USE_PROFILER}>,Tracy::TracyClient,>
)


target_compile_features(${PROJECT_NAME}_profile
	PRIVATE
	cxx_std_23
)

add_library(${PROJECT_NAME}::profile ALIAS ${PROJECT_NAME}_profile)
