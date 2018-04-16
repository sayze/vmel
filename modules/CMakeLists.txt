cmake_minimum_required(VERSION 3.0.0)

# Set project config.
project(vmel_modules)

set(CMAKE_BUILD_TYPE Release)
set(BUILD_TYPE SHARED)
set(PROJ_SRC_DIR src)
set(SOURCES vstring.c)

option(BUILD_STAT_LIB "Build a static library" OFF)

if(BUILD_STAT_LIB)
	set(BUILD_TYPE STATIC)	
endif(BUILD_STAT_LIB)
unset(BUILD_STAT_LIB CACHE)

# Bring the headers.
include_directories(include)

message("Building: " ${CMAKE_BUILD_TYPE})

foreach(source ${SOURCES})
	string(REGEX REPLACE "\\.[^.]*$" "" sourcef ${source})
	#Generate the shared library from the sources
	add_library(${sourcef} ${BUILD_TYPE} ${PROJ_SRC_DIR}/${source})
endforeach()

