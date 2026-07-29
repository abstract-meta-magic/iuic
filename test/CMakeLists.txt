cmake_minimum_required(VERSION 3.31)

add_library(${PROJECT_NAME}_test STATIC)

if(TARGET iuic_std_module)
target_link_libraries(${PROJECT_NAME}_test iuic_std_module)
endif()

target_compile_features(${PROJECT_NAME}_test
	PRIVATE cxx_std_23
)


target_sources(${PROJECT_NAME}_test PUBLIC
	FILE_SET iuic_test_modules 
	TYPE CXX_MODULES
	FILES
	"src/iuic-test.cppm"
)
