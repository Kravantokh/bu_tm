cmake_minimum_required(VERSION 3.22.2)

project(bu_terminal_manager VERSION 1.0)

set(CMAKE_C_STANDARD 90)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(BU_TERMINAL_MANAGER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/ PARENT_SCOPE)
set(BU_TERMINAL_MANAGER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/)


add_subdirectory(dependencies/benutils_unicode)

include_directories(${BU_TERMINAL_MANAGER_INCLUDE} ${BU_UNICODE_INCLUDE})

add_library(bu_terminal_manager STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_manager.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/tm_elements.c
)

add_executable(test
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test.c
	)

target_link_libraries(test bu_terminal_manager bu_unicode)
