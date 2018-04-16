cmake_minimum_required(VERSION 3.0.0)

# Set project config.
project(vmel)

link_directories(modules/build/)

set(FSOURCES)
set(PROJ_SRC_DIR src)
set(SOURCES errors.c nexec.c node.c parser.c sytable.c tokenizer.c utils.c vmel.c)

message("Building: " ${CMAKE_BUILD_TYPE})

# Bring the headers.
include_directories(include modules/include)

foreach(source ${SOURCES})
	list(APPEND FSOURCES ${PROJ_SRC_DIR}/${source})
endforeach()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	list(APPEND FSOURCES modules/src/vstring.c)
endif()

add_executable(vmel ${FSOURCES})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_link_libraries(vmel vstring)
endif()
