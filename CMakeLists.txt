cmake_minimum_required(VERSION 3.31)

project(iuic
	VERSION 0.3.0
	LANGUAGES CXX
)

option(IUIC_BUILD_TESTS "Build unit tests" ON)
option(IUIC_EXTRA "Add extra flags" OFF)

set(IUIC_DEFAULT_TARGET_COMPILE_OPTIONS
	# DEBUG
	$<$<CONFIG:Debug>:-g>
	$<$<CONFIG:Debug>:-O0>
	# RELEASE
	$<$<CONFIG:Release>:$<IF:$<BOOL:${IUIC_EXTRA}>,-O3,-O2>>
)



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
