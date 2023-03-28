add_library(heaters INTERFACE)

add_compile_options(-Wall -Wextra -pedantic -Werror)

target_include_directories(heaters INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(heaters INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/heaters.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/heater.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/heater_frame.cpp)
