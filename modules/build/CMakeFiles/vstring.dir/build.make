# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.10.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.10.3/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/sayedsadeed/projects/vmel/modules

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/sayedsadeed/projects/vmel/modules/build

# Include any dependencies generated for this target.
include CMakeFiles/vstring.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/vstring.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/vstring.dir/flags.make

CMakeFiles/vstring.dir/src/vstring.c.o: CMakeFiles/vstring.dir/flags.make
CMakeFiles/vstring.dir/src/vstring.c.o: ../src/vstring.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/sayedsadeed/projects/vmel/modules/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/vstring.dir/src/vstring.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/vstring.dir/src/vstring.c.o   -c /Users/sayedsadeed/projects/vmel/modules/src/vstring.c

CMakeFiles/vstring.dir/src/vstring.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vstring.dir/src/vstring.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/sayedsadeed/projects/vmel/modules/src/vstring.c > CMakeFiles/vstring.dir/src/vstring.c.i

CMakeFiles/vstring.dir/src/vstring.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vstring.dir/src/vstring.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/sayedsadeed/projects/vmel/modules/src/vstring.c -o CMakeFiles/vstring.dir/src/vstring.c.s

CMakeFiles/vstring.dir/src/vstring.c.o.requires:

.PHONY : CMakeFiles/vstring.dir/src/vstring.c.o.requires

CMakeFiles/vstring.dir/src/vstring.c.o.provides: CMakeFiles/vstring.dir/src/vstring.c.o.requires
	$(MAKE) -f CMakeFiles/vstring.dir/build.make CMakeFiles/vstring.dir/src/vstring.c.o.provides.build
.PHONY : CMakeFiles/vstring.dir/src/vstring.c.o.provides

CMakeFiles/vstring.dir/src/vstring.c.o.provides.build: CMakeFiles/vstring.dir/src/vstring.c.o


# Object files for target vstring
vstring_OBJECTS = \
"CMakeFiles/vstring.dir/src/vstring.c.o"

# External object files for target vstring
vstring_EXTERNAL_OBJECTS =

libvstring.dylib: CMakeFiles/vstring.dir/src/vstring.c.o
libvstring.dylib: CMakeFiles/vstring.dir/build.make
libvstring.dylib: CMakeFiles/vstring.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/sayedsadeed/projects/vmel/modules/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library libvstring.dylib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vstring.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/vstring.dir/build: libvstring.dylib

.PHONY : CMakeFiles/vstring.dir/build

CMakeFiles/vstring.dir/requires: CMakeFiles/vstring.dir/src/vstring.c.o.requires

.PHONY : CMakeFiles/vstring.dir/requires

CMakeFiles/vstring.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/vstring.dir/cmake_clean.cmake
.PHONY : CMakeFiles/vstring.dir/clean

CMakeFiles/vstring.dir/depend:
	cd /Users/sayedsadeed/projects/vmel/modules/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/sayedsadeed/projects/vmel/modules /Users/sayedsadeed/projects/vmel/modules /Users/sayedsadeed/projects/vmel/modules/build /Users/sayedsadeed/projects/vmel/modules/build /Users/sayedsadeed/projects/vmel/modules/build/CMakeFiles/vstring.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/vstring.dir/depend
