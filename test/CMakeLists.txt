# https://crascit.com/2015/07/25/cmake-gtest/

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

set(TEST_TARGETS PARENT_SCOPE)

macro(register_test NAME DIR)
	add_executable(${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/test.cpp)
	target_link_libraries(${NAME} gtest_main heaters)
	target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

	add_test(NAME ${NAME} COMMAND ${NAME})

    list(APPEND TEST_TARGETS ${NAME})
    set(TEST_TARGETS ${TEST_TARGETS} PARENT_SCOPE)
endmacro()

register_test(heaters_test local_heaters)
