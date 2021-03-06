cmake_minimum_required(VERSION 3.13)

project(libqif)

include(../misc/cmake/BackwardCpp.cmake)

add_custom_target(tests_cpp)											# for 'make tests_cpp'

if(EXISTS "../external/googletest/CMakeLists.txt")						# only if googletest is checked out

	add_subdirectory(../external/googletest googletest)					# build googletest in dir 'googletest'

	file(GLOB_RECURSE TEST_SOURCES *.cpp)								# get all *.cpp files in TEST_SOURCES

	add_executable(run ${TEST_SOURCES} ${BACKWARD_ENABLE})				# create 'run' executable
	target_link_libraries(run qif_cpp gtest)							# link against libqif and googletest
	target_include_directories(run PRIVATE include)
	add_backward(run)													# add stuff required by backward-cpp

	add_dependencies(tests_cpp run)
	add_test(NAME tests_cpp COMMAND run)

endif()
