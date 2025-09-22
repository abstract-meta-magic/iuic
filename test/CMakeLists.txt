cmake_minimum_required(VERSION 3.30)

find_package(SDL3 REQUIRED)

add_executable(te base.cpp)


## xxHash - START
add_library(xxLib STATIC IMPORTED)

set_target_properties(xxLib PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/tmp_solutions/hash/libxxhash.a)


target_link_libraries(te xxLib)
## xxHash - END

target_link_libraries(te iuic::core SDL3::SDL3)

# -Wall -Wextra
target_compile_options(te PRIVATE  -O0 -fcolor-diagnostics)

if(IUIC_TEST_TYPE STREQUAL "ADDRESS")
	message("Address test mode")
target_compile_options(te PRIVATE  -fsanitize=address)
target_link_options(te PRIVATE -fsanitize=address)
elseif(IUIC_TEST_TYPE STREQUAL "UB")
	message("UB test mode")
target_compile_options(te PRIVATE  -fsanitize=undefined)
target_link_options(te PRIVATE -fsanitize=undefined)
endif()


