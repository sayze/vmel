cmake_minimum_required(VERSION 2.8.9)

# Set project config.
project(vmel_modules)

set(CMAKE_BUILD_TYPE Release)

set(FSOURCES)
set(PROJ_SRC_DIR src)
set(SOURCES vstring.c)

# Bring the headers.
include_directories(include)

message("Building: " ${CMAKE_BUILD_TYPE})

foreach(source ${SOURCES})
	string(REGEX REPLACE "\\.[^.]*$" "" sourcef ${source})
	#Generate the shared library from the sources
	add_library(${sourcef} SHARED src/${source})
endforeach()

