cmake_minimum_required(VERSION 2.8.9)

# Set project config.
project(vmel)

set(FSOURCES)
set(PROJ_SRC_DIR src)
set(SOURCES errors.c nexec.c node.c parser.c sytable.c tokenizer.c utils.c vmel.c)

message("Building: " ${CMAKE_BUILD_TYPE})

# Bring the headers.
include_directories(include)

foreach(source ${SOURCES})
	list(APPEND FSOURCES ${PROJ_SRC_DIR}/${source})
endforeach()

add_executable(vmel ${FSOURCES})