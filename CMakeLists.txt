cmake_minimum_required(VERSION 3.22.2)

project(bu_terminal_manager VERSION 1.0)

set(CMAKE_C_STANDARD 90)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Propagating include dirs from previous level
set(BU_TERMINAL_MANAGER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/benutils_unicode)

include_directories(${BU_TERMINAL_MANAGER_INCLUDE} ${BU_UNICODE_INCLUDE})

# Propagating include dirs to next level
set(BU_TERMINAL_MANAGER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/ PARENT_SCOPE)
set(BU_UNICODE_INCLUDE ${BU_UNICODE_INCLUDE} PARENT_SCOPE)

add_library(bu_terminal_manager 
	${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_manager.c
	)
target_link_libraries(bu_terminal_manager bu_unicode)
