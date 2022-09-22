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

add_compile_options(-ggdb3 -gstabs -O0 )#-fanalyzer -fsanitize=address)
#add_link_options(-fanalyzer -fsanitize=address)

target_link_libraries(bu_terminal_manager bu_unicode)

add_executable(test
	${CMAKE_CURRENT_SOURCE_DIR}/tests/high_level.c
	)


target_link_libraries(test bu_terminal_manager bu_unicode)
