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
include CMakeFiles/mcf_levelsets2d.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/mcf_levelsets2d.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mcf_levelsets2d.dir/flags.make

CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o: CMakeFiles/mcf_levelsets2d.dir/flags.make
CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o: ../mcf_levelsets2d.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/andesil/PSB/lib/CImg-1.5.8/examples/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o -c /home/andesil/PSB/lib/CImg-1.5.8/examples/mcf_levelsets2d.cpp

CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/andesil/PSB/lib/CImg-1.5.8/examples/mcf_levelsets2d.cpp > CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.i

CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/andesil/PSB/lib/CImg-1.5.8/examples/mcf_levelsets2d.cpp -o CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.s

CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o.requires:
.PHONY : CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o.requires

CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o.provides: CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o.requires
	$(MAKE) -f CMakeFiles/mcf_levelsets2d.dir/build.make CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o.provides.build
.PHONY : CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o.provides

CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o.provides.build: CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o

# Object files for target mcf_levelsets2d
mcf_levelsets2d_OBJECTS = \
"CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o"

# External object files for target mcf_levelsets2d
mcf_levelsets2d_EXTERNAL_OBJECTS =

../mcf_levelsets2d: CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o
../mcf_levelsets2d: CMakeFiles/mcf_levelsets2d.dir/build.make
../mcf_levelsets2d: /usr/lib64/libtiff.so
../mcf_levelsets2d: /usr/lib64/libjpeg.so
../mcf_levelsets2d: /usr/lib64/libz.so
../mcf_levelsets2d: /usr/lib64/libpng.so
../mcf_levelsets2d: /usr/lib64/libz.so
../mcf_levelsets2d: /usr/lib64/libSM.so
../mcf_levelsets2d: /usr/lib64/libICE.so
../mcf_levelsets2d: /usr/lib64/libX11.so
../mcf_levelsets2d: /usr/lib64/libXext.so
../mcf_levelsets2d: /usr/lib64/liblapack.so
../mcf_levelsets2d: /usr/lib64/blas/reference/libblas.so
../mcf_levelsets2d: /usr/lib64/blas/reference/libblas.so
../mcf_levelsets2d: /usr/lib64/libpng.so
../mcf_levelsets2d: /usr/lib64/libSM.so
../mcf_levelsets2d: /usr/lib64/libICE.so
../mcf_levelsets2d: /usr/lib64/libX11.so
../mcf_levelsets2d: /usr/lib64/libXext.so
../mcf_levelsets2d: /usr/lib64/liblapack.so
../mcf_levelsets2d: /usr/lib64/blas/reference/libblas.so
../mcf_levelsets2d: CMakeFiles/mcf_levelsets2d.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../mcf_levelsets2d"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mcf_levelsets2d.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mcf_levelsets2d.dir/build: ../mcf_levelsets2d
.PHONY : CMakeFiles/mcf_levelsets2d.dir/build

CMakeFiles/mcf_levelsets2d.dir/requires: CMakeFiles/mcf_levelsets2d.dir/mcf_levelsets2d.cpp.o.requires
.PHONY : CMakeFiles/mcf_levelsets2d.dir/requires

CMakeFiles/mcf_levelsets2d.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mcf_levelsets2d.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mcf_levelsets2d.dir/clean

CMakeFiles/mcf_levelsets2d.dir/depend:
	cd /home/andesil/PSB/lib/CImg-1.5.8/examples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/andesil/PSB/lib/CImg-1.5.8/examples /home/andesil/PSB/lib/CImg-1.5.8/examples /home/andesil/PSB/lib/CImg-1.5.8/examples/build /home/andesil/PSB/lib/CImg-1.5.8/examples/build /home/andesil/PSB/lib/CImg-1.5.8/examples/build/CMakeFiles/mcf_levelsets2d.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mcf_levelsets2d.dir/depend

