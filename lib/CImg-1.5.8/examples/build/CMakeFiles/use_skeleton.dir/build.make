# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/andesil/PSB/lib/CImg-1.5.8/examples

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/andesil/PSB/lib/CImg-1.5.8/examples/build

# Include any dependencies generated for this target.
include CMakeFiles/use_skeleton.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/use_skeleton.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/use_skeleton.dir/flags.make

CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o: CMakeFiles/use_skeleton.dir/flags.make
CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o: ../use_skeleton.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/andesil/PSB/lib/CImg-1.5.8/examples/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o -c /home/andesil/PSB/lib/CImg-1.5.8/examples/use_skeleton.cpp

CMakeFiles/use_skeleton.dir/use_skeleton.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/use_skeleton.dir/use_skeleton.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/andesil/PSB/lib/CImg-1.5.8/examples/use_skeleton.cpp > CMakeFiles/use_skeleton.dir/use_skeleton.cpp.i

CMakeFiles/use_skeleton.dir/use_skeleton.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/use_skeleton.dir/use_skeleton.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/andesil/PSB/lib/CImg-1.5.8/examples/use_skeleton.cpp -o CMakeFiles/use_skeleton.dir/use_skeleton.cpp.s

CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o.requires:
.PHONY : CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o.requires

CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o.provides: CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o.requires
	$(MAKE) -f CMakeFiles/use_skeleton.dir/build.make CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o.provides.build
.PHONY : CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o.provides

CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o.provides.build: CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o

# Object files for target use_skeleton
use_skeleton_OBJECTS = \
"CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o"

# External object files for target use_skeleton
use_skeleton_EXTERNAL_OBJECTS =

../use_skeleton: CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o
../use_skeleton: CMakeFiles/use_skeleton.dir/build.make
../use_skeleton: /usr/lib64/libtiff.so
../use_skeleton: /usr/lib64/libjpeg.so
../use_skeleton: /usr/lib64/libz.so
../use_skeleton: /usr/lib64/libpng.so
../use_skeleton: /usr/lib64/libz.so
../use_skeleton: /usr/lib64/libSM.so
../use_skeleton: /usr/lib64/libICE.so
../use_skeleton: /usr/lib64/libX11.so
../use_skeleton: /usr/lib64/libXext.so
../use_skeleton: /usr/lib64/liblapack.so
../use_skeleton: /usr/lib64/blas/reference/libblas.so
../use_skeleton: /usr/lib64/blas/reference/libblas.so
../use_skeleton: /usr/lib64/libpng.so
../use_skeleton: /usr/lib64/libSM.so
../use_skeleton: /usr/lib64/libICE.so
../use_skeleton: /usr/lib64/libX11.so
../use_skeleton: /usr/lib64/libXext.so
../use_skeleton: /usr/lib64/liblapack.so
../use_skeleton: /usr/lib64/blas/reference/libblas.so
../use_skeleton: CMakeFiles/use_skeleton.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../use_skeleton"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/use_skeleton.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/use_skeleton.dir/build: ../use_skeleton
.PHONY : CMakeFiles/use_skeleton.dir/build

CMakeFiles/use_skeleton.dir/requires: CMakeFiles/use_skeleton.dir/use_skeleton.cpp.o.requires
.PHONY : CMakeFiles/use_skeleton.dir/requires

CMakeFiles/use_skeleton.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/use_skeleton.dir/cmake_clean.cmake
.PHONY : CMakeFiles/use_skeleton.dir/clean

CMakeFiles/use_skeleton.dir/depend:
	cd /home/andesil/PSB/lib/CImg-1.5.8/examples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/andesil/PSB/lib/CImg-1.5.8/examples /home/andesil/PSB/lib/CImg-1.5.8/examples /home/andesil/PSB/lib/CImg-1.5.8/examples/build /home/andesil/PSB/lib/CImg-1.5.8/examples/build /home/andesil/PSB/lib/CImg-1.5.8/examples/build/CMakeFiles/use_skeleton.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/use_skeleton.dir/depend

