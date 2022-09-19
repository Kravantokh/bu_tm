cmake_minimum_required(VERSION 3.22.2)

project(bu_terminal_manager VERSION 1.0)

set(CMAKE_C_STANDARD 90)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/benutils_unicode)

add_library(bu_terminal_manager STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_manager.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/tm_elements.c
)

target_link_libraries(bu_terminal_manager bu_unicode)

add_executable(test
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test.c
	)

target_link_libraries(test bu_terminal_manager bu_unicode)
