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
include CMakeFiles/plotter1d.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/plotter1d.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/plotter1d.dir/flags.make

CMakeFiles/plotter1d.dir/plotter1d.cpp.o: CMakeFiles/plotter1d.dir/flags.make
CMakeFiles/plotter1d.dir/plotter1d.cpp.o: ../plotter1d.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/andesil/PSB/lib/CImg-1.5.8/examples/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/plotter1d.dir/plotter1d.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/plotter1d.dir/plotter1d.cpp.o -c /home/andesil/PSB/lib/CImg-1.5.8/examples/plotter1d.cpp

CMakeFiles/plotter1d.dir/plotter1d.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/plotter1d.dir/plotter1d.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/andesil/PSB/lib/CImg-1.5.8/examples/plotter1d.cpp > CMakeFiles/plotter1d.dir/plotter1d.cpp.i

CMakeFiles/plotter1d.dir/plotter1d.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/plotter1d.dir/plotter1d.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/andesil/PSB/lib/CImg-1.5.8/examples/plotter1d.cpp -o CMakeFiles/plotter1d.dir/plotter1d.cpp.s

CMakeFiles/plotter1d.dir/plotter1d.cpp.o.requires:
.PHONY : CMakeFiles/plotter1d.dir/plotter1d.cpp.o.requires

CMakeFiles/plotter1d.dir/plotter1d.cpp.o.provides: CMakeFiles/plotter1d.dir/plotter1d.cpp.o.requires
	$(MAKE) -f CMakeFiles/plotter1d.dir/build.make CMakeFiles/plotter1d.dir/plotter1d.cpp.o.provides.build
.PHONY : CMakeFiles/plotter1d.dir/plotter1d.cpp.o.provides

CMakeFiles/plotter1d.dir/plotter1d.cpp.o.provides.build: CMakeFiles/plotter1d.dir/plotter1d.cpp.o

# Object files for target plotter1d
plotter1d_OBJECTS = \
"CMakeFiles/plotter1d.dir/plotter1d.cpp.o"

# External object files for target plotter1d
plotter1d_EXTERNAL_OBJECTS =

../plotter1d: CMakeFiles/plotter1d.dir/plotter1d.cpp.o
../plotter1d: CMakeFiles/plotter1d.dir/build.make
../plotter1d: /usr/lib64/libtiff.so
../plotter1d: /usr/lib64/libjpeg.so
../plotter1d: /usr/lib64/libz.so
../plotter1d: /usr/lib64/libpng.so
../plotter1d: /usr/lib64/libz.so
../plotter1d: /usr/lib64/libSM.so
../plotter1d: /usr/lib64/libICE.so
../plotter1d: /usr/lib64/libX11.so
../plotter1d: /usr/lib64/libXext.so
../plotter1d: /usr/lib64/liblapack.so
../plotter1d: /usr/lib64/blas/reference/libblas.so
../plotter1d: /usr/lib64/blas/reference/libblas.so
../plotter1d: /usr/lib64/libpng.so
../plotter1d: /usr/lib64/libSM.so
../plotter1d: /usr/lib64/libICE.so
../plotter1d: /usr/lib64/libX11.so
../plotter1d: /usr/lib64/libXext.so
../plotter1d: /usr/lib64/liblapack.so
../plotter1d: /usr/lib64/blas/reference/libblas.so
../plotter1d: CMakeFiles/plotter1d.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../plotter1d"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/plotter1d.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/plotter1d.dir/build: ../plotter1d
.PHONY : CMakeFiles/plotter1d.dir/build

CMakeFiles/plotter1d.dir/requires: CMakeFiles/plotter1d.dir/plotter1d.cpp.o.requires
.PHONY : CMakeFiles/plotter1d.dir/requires

CMakeFiles/plotter1d.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/plotter1d.dir/cmake_clean.cmake
.PHONY : CMakeFiles/plotter1d.dir/clean

CMakeFiles/plotter1d.dir/depend:
	cd /home/andesil/PSB/lib/CImg-1.5.8/examples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/andesil/PSB/lib/CImg-1.5.8/examples /home/andesil/PSB/lib/CImg-1.5.8/examples /home/andesil/PSB/lib/CImg-1.5.8/examples/build /home/andesil/PSB/lib/CImg-1.5.8/examples/build /home/andesil/PSB/lib/CImg-1.5.8/examples/build/CMakeFiles/plotter1d.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/plotter1d.dir/depend

