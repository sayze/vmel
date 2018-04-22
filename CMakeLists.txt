cmake_minimum_required(VERSION 3.0.0)

# Set project config.
project(vmel)

# Final sources with prepended path
set(FSOURCES)

# Main src directory
set(PROJ_SRC_DIR src)

# Modules directory
set(MOD_SRC_DIR modules/src)

# Debug flag


# Souce files for modules and main
set(SOURCES errors.c nexec.c node.c parser.c sytable.c tokenizer.c utils.c vmel.c)
set(MODSRC vstring.c)

message("Building: " ${CMAKE_BUILD_TYPE})

# Includes
include_directories(include modules/include)

foreach(source ${SOURCES})
	list(APPEND FSOURCES ${PROJ_SRC_DIR}/${source})
endforeach()

foreach(msource ${MODSRC})
	list(APPEND FSOURCES ${MOD_SRC_DIR}/${msource})
endforeach()

add_executable(vmel ${FSOURCES})
