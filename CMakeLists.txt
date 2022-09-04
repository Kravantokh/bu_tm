cmake_minimum_required(VERSION 3.22.2)

project(bu_terminal_manager VERSION 1.0)

set(CMAKE_C_STANDARD 90)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(BU_TERMINAL_MANAGER_INCLUDE ${PROJECT_SOURCE_DIR}/include/ PARENT_SCOPE)
set(BU_TERMINAL_MANAGER_INCLUDE ${PROJECT_SOURCE_DIR}/include/)


add_subdirectory(dependencies/benutils_unicode)

add_library(bu_terminal_manager 
	${PROJECT_SOURCE_DIR}/src/terminal_manager.c
	)

include_directories(${BU_TERMINAL_MANAGER_INCLUDE} ${BU_UNICODE_INCLUDE})
