cmake_minimum_required(VERSION 4.0)

project(iuic
	VERSION 0.3.2
	LANGUAGES CXX
)

option(IUIC_USER_STD "Using user difined module std[iuic::user_std]" OFF)
option(IUIC_BUILD_TESTS "Build unit tests" OFF)
option(IUIC_EXTRA "Add extra flags" OFF)

set(IUIC_DEFAULT_TARGET_COMPILE_OPTIONS
	# DEBUG
	$<$<CONFIG:Debug>:-g>
	$<$<CONFIG:Debug>:-O0>
	$<$<CONFIG:Debug>:-Wall>
	$<$<CONFIG:Debug>:-Wextra>
	# RELEASE
	$<$<CONFIG:Release>:$<IF:$<BOOL:${IUIC_EXTRA}>,-O3,-O2>>
)

if(${IUIC_USER_STD} AND NOT TARGET iuic::user_std)
	message(FATAL_ERROR "IUIC_USER_STD=ON, but target iuic::user_std is not defined")
endif()



## project ENV ?

## lvl - 0
add_subdirectory(test)
add_subdirectory(underlying)
## lvl - 1
add_subdirectory(text)
add_subdirectory(state)
add_subdirectory(style)
add_subdirectory(events)
## lvl - 3
add_subdirectory(env)
add_subdirectory(layout)
## Main
add_subdirectory(core)
