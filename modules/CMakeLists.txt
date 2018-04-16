cmake_minimum_required(VERSION 2.8.9)

# Set project config.
project(vmel)

set(FSOURCES)
set(PROJ_SRC_DIR src)
set(SOURCES vstring.c)

# Bring the headers.
include_directories(include)

foreach(source ${SOURCES})
	string(REGEX REPLACE "\\.[^.]*$" "" sourcef ${source})
	#Generate the shared library from the sources
	add_library(${sourcef} SHARED src/${source})
 	install(TARGETS ${sourcef} DESTINATION ${sourcef})
endforeach()

