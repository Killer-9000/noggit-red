# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Volumes/APFS/NoggitQT

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Volumes/APFS/NoggitQT/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/math-matrix_4x4.test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/math-matrix_4x4.test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/math-matrix_4x4.test.dir/flags.make

CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.o: CMakeFiles/math-matrix_4x4.test.dir/flags.make
CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.o: ../test/math/matrix_4x4.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Volumes/APFS/NoggitQT/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.o -c /Volumes/APFS/NoggitQT/test/math/matrix_4x4.cpp

CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Volumes/APFS/NoggitQT/test/math/matrix_4x4.cpp > CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.i

CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Volumes/APFS/NoggitQT/test/math/matrix_4x4.cpp -o CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.s

# Object files for target math-matrix_4x4.test
math__matrix_4x4_test_OBJECTS = \
"CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.o"

# External object files for target math-matrix_4x4.test
math__matrix_4x4_test_EXTERNAL_OBJECTS =

bin/math-matrix_4x4.test: CMakeFiles/math-matrix_4x4.test.dir/test/math/matrix_4x4.cpp.o
bin/math-matrix_4x4.test: CMakeFiles/math-matrix_4x4.test.dir/build.make
bin/math-matrix_4x4.test: /usr/local/lib/libboost_unit_test_framework-mt.a
bin/math-matrix_4x4.test: libnoggit-math.a
bin/math-matrix_4x4.test: CMakeFiles/math-matrix_4x4.test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Volumes/APFS/NoggitQT/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/math-matrix_4x4.test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/math-matrix_4x4.test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/math-matrix_4x4.test.dir/build: bin/math-matrix_4x4.test

.PHONY : CMakeFiles/math-matrix_4x4.test.dir/build

CMakeFiles/math-matrix_4x4.test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/math-matrix_4x4.test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/math-matrix_4x4.test.dir/clean

CMakeFiles/math-matrix_4x4.test.dir/depend:
	cd /Volumes/APFS/NoggitQT/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Volumes/APFS/NoggitQT /Volumes/APFS/NoggitQT /Volumes/APFS/NoggitQT/cmake-build-debug /Volumes/APFS/NoggitQT/cmake-build-debug /Volumes/APFS/NoggitQT/cmake-build-debug/CMakeFiles/math-matrix_4x4.test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/math-matrix_4x4.test.dir/depend

