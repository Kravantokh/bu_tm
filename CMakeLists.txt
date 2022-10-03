cmake_minimum_required(VERSION 3.22.2)

set(CMAKE_C_STANDARD 90)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

add_compile_options(-g -pg)
add_link_options(-g -pg)

project(bu_terminal_manager VERSION 1.0)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/benutils_unicode)

add_library(bu_terminal_manager STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/src/terminal_manager.c
)

target_link_libraries(bu_terminal_manager bu_unicode)


# Should the examples be built?
if(${BUILD_EXAMPLES})
	add_executable(high_level_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/high_level.c)
	#add_executable(mid_level_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/mid_level.c)
	#add_executable(low_level_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/low_level.c)

	target_link_libraries(high_level_example bu_terminal_manager bu_unicode)
	#target_link_libraries(mid_level_example bu_terminal_manager bu_unicode)
	#target_link_libraries(low_level_example bu_terminal_manager bu_unicode)
endif(${BUILD_EXAMPLES})

