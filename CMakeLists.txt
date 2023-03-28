cmake_minimum_required(VERSION 3.17)

project(Heaters CXX C ASM)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

#add_compile_options(-Wall -Wextra -pedantic -Werror -Werror=unused-parameter)

enable_testing()

if (NOT DEFINED TEST_TARGETS)
	set(TEST_TARGETS "")
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

add_custom_target(check_${PROJECT_NAME}
		COMMAND env CTEST_OUTPUT_ON_FAILURE=1 GTEST_COLOR=1 ${CMAKE_CTEST_COMMAND}
		DEPENDS ${TEST_TARGETS})
