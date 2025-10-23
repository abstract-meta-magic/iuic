

add_library(xxLib STATIC IMPORTED)

set_target_properties(xxLib PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/tmp_solutions/hash/libxxhash.a)



add_executable(button button.cpp)
target_link_libraries(button PRIVATE iuic::core xxLib)


add_executable(checkbox checkbox.cpp)
target_link_libraries(checkbox PRIVATE iuic::core xxLib)

