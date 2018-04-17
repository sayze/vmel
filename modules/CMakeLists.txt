cmake_minimum_required(VERSION 3.0.0)

# Set project config.
project(vmel_modules)

# Build type default to release
set(CMAKE_BUILD_TYPE Release)

# Flag for build type
set(BUILD_TYPE SHARED)

# Sources
set(PROJ_SRC_DIR src)
set(SOURCES vstring.c)

# Set default build to shared.
option(BUILD_STAT_LIB "Build static library" OFF)

# Update build type if provided.
if(BUILD_STAT_LIB)
	set(BUILD_TYPE STATIC)	
endif(BUILD_STAT_LIB)
unset(BUILD_STAT_LIB CACHE)

# Header files.
include_directories(include)

message("Building: " ${CMAKE_BUILD_TYPE})

# Foreach individual library remove file extension.
# Then create library for that perticular file.
foreach(source ${SOURCES})
	string(REGEX REPLACE "\\.[^.]*$" "" sourcef ${source})
	add_library(${sourcef} ${BUILD_TYPE} ${PROJ_SRC_DIR}/${source})
endforeach()

