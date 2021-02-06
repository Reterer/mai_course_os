cmake_minimum_required( VERSION 3.10 )

project( lab2 LANGUAGES C )
set( CMAKE_C_FLAGS "-Wall -pedantic -std=c99" )
set( COMMON_FILES src/io.c src/io.h )

add_executable( parent src/parent.c ${COMMON_FILES} )
add_executable( child src/child.c ${COMMON_FILES} )
