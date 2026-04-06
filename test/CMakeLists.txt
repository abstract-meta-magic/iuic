cmake_minimum_required(VERSION 3.30)

find_package(Raylib REQUIRED)
find_package(Tracy REQUIRED)


add_executable(te base.cpp)


target_link_libraries(te iuic::core kitty_kit raylib Tracy::TracyClient)

if(IUIC_TMP_CXX_STD)
	#	target_link_libraries(te tmp_cxx_std)
endif()
## xxHash - END


# -Wall -Wextra
target_compile_options(te PRIVATE  -O0 -fcolor-diagnostics )

if(IUIC_TEST_TYPE STREQUAL "ADDRESS")
	message("Address test mode")
target_compile_options(te PRIVATE  -fsanitize=address)
target_link_options(te PRIVATE -fsanitize=address)
elseif(IUIC_TEST_TYPE STREQUAL "UB")
	message("UB test mode")
target_compile_options(te PRIVATE  -fsanitize=undefined)
target_link_options(te PRIVATE -fsanitize=undefined)
endif()


