cmake_minimum_required(VERSION 4.0)

project(iuic
	VERSION 0.6.4
	LANGUAGES CXX
)

option(IUIC_USER_STD "Using user difined module std[iuic::user_std]" OFF)
option(IUIC_BUILD_TESTS "Build unit tests" OFF)
option(IUIC_USE_PROFILER "Porfiler[private: tracy]" OFF)
option(IUIC_EXTRA "Add extra flags" OFF)

if(${IUIC_USER_STD} AND NOT TARGET iuic::user_std)
	message(FATAL_ERROR "IUIC_USER_STD=ON, but target iuic::user_std is not defined")
endif()


add_library(${PROJECT_NAME}_default INTERFACE)

target_compile_options(${PROJECT_NAME}_default
	INTERFACE
	$<$<CONFIG:Debug>:-g>
	$<$<CONFIG:Debug>:-O0>
	$<$<CONFIG:Debug>:-Wall>
	$<$<CONFIG:Debug>:-Wextra>
)

target_link_libraries(${PROJECT_NAME}_default
	INTERFACE $<IF:$<BOOL:${IUIC_USER_STD}>,iuic::user_std,>
)


## project ENV ?

## lvl - 0
add_subdirectory(profile)
add_subdirectory(test)
add_subdirectory(underlying)
## lvl - 1
add_subdirectory(asset)
add_subdirectory(text)
add_subdirectory(state)
add_subdirectory(style)
add_subdirectory(events)
## lvl - 3
add_subdirectory(env)
add_subdirectory(layout)
## Main
add_subdirectory(core)
