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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/fwq/111/bloomfilter_v2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fwq/111/bloomfilter_v2

# Include any dependencies generated for this target.
include CMakeFiles/TestIterator.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/TestIterator.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TestIterator.dir/flags.make

CMakeFiles/TestIterator.dir/test/testIterator.cc.o: CMakeFiles/TestIterator.dir/flags.make
CMakeFiles/TestIterator.dir/test/testIterator.cc.o: test/testIterator.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fwq/111/bloomfilter_v2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TestIterator.dir/test/testIterator.cc.o"
	/usr/bin/g++-9  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TestIterator.dir/test/testIterator.cc.o -c /home/fwq/111/bloomfilter_v2/test/testIterator.cc

CMakeFiles/TestIterator.dir/test/testIterator.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TestIterator.dir/test/testIterator.cc.i"
	/usr/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fwq/111/bloomfilter_v2/test/testIterator.cc > CMakeFiles/TestIterator.dir/test/testIterator.cc.i

CMakeFiles/TestIterator.dir/test/testIterator.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TestIterator.dir/test/testIterator.cc.s"
	/usr/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fwq/111/bloomfilter_v2/test/testIterator.cc -o CMakeFiles/TestIterator.dir/test/testIterator.cc.s

CMakeFiles/TestIterator.dir/src/kvstore.cc.o: CMakeFiles/TestIterator.dir/flags.make
CMakeFiles/TestIterator.dir/src/kvstore.cc.o: src/kvstore.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fwq/111/bloomfilter_v2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/TestIterator.dir/src/kvstore.cc.o"
	/usr/bin/g++-9  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TestIterator.dir/src/kvstore.cc.o -c /home/fwq/111/bloomfilter_v2/src/kvstore.cc

CMakeFiles/TestIterator.dir/src/kvstore.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TestIterator.dir/src/kvstore.cc.i"
	/usr/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fwq/111/bloomfilter_v2/src/kvstore.cc > CMakeFiles/TestIterator.dir/src/kvstore.cc.i

CMakeFiles/TestIterator.dir/src/kvstore.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TestIterator.dir/src/kvstore.cc.s"
	/usr/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fwq/111/bloomfilter_v2/src/kvstore.cc -o CMakeFiles/TestIterator.dir/src/kvstore.cc.s

CMakeFiles/TestIterator.dir/src/bloomfilter.cc.o: CMakeFiles/TestIterator.dir/flags.make
CMakeFiles/TestIterator.dir/src/bloomfilter.cc.o: src/bloomfilter.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fwq/111/bloomfilter_v2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/TestIterator.dir/src/bloomfilter.cc.o"
	/usr/bin/g++-9  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TestIterator.dir/src/bloomfilter.cc.o -c /home/fwq/111/bloomfilter_v2/src/bloomfilter.cc

CMakeFiles/TestIterator.dir/src/bloomfilter.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TestIterator.dir/src/bloomfilter.cc.i"
	/usr/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fwq/111/bloomfilter_v2/src/bloomfilter.cc > CMakeFiles/TestIterator.dir/src/bloomfilter.cc.i

CMakeFiles/TestIterator.dir/src/bloomfilter.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TestIterator.dir/src/bloomfilter.cc.s"
	/usr/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fwq/111/bloomfilter_v2/src/bloomfilter.cc -o CMakeFiles/TestIterator.dir/src/bloomfilter.cc.s

CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.o: CMakeFiles/TestIterator.dir/flags.make
CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.o: src/MurmurHash3.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fwq/111/bloomfilter_v2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.o"
	/usr/bin/g++-9  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.o -c /home/fwq/111/bloomfilter_v2/src/MurmurHash3.cc

CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.i"
	/usr/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fwq/111/bloomfilter_v2/src/MurmurHash3.cc > CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.i

CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.s"
	/usr/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fwq/111/bloomfilter_v2/src/MurmurHash3.cc -o CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.s

# Object files for target TestIterator
TestIterator_OBJECTS = \
"CMakeFiles/TestIterator.dir/test/testIterator.cc.o" \
"CMakeFiles/TestIterator.dir/src/kvstore.cc.o" \
"CMakeFiles/TestIterator.dir/src/bloomfilter.cc.o" \
"CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.o"

# External object files for target TestIterator
TestIterator_EXTERNAL_OBJECTS =

TestIterator: CMakeFiles/TestIterator.dir/test/testIterator.cc.o
TestIterator: CMakeFiles/TestIterator.dir/src/kvstore.cc.o
TestIterator: CMakeFiles/TestIterator.dir/src/bloomfilter.cc.o
TestIterator: CMakeFiles/TestIterator.dir/src/MurmurHash3.cc.o
TestIterator: CMakeFiles/TestIterator.dir/build.make
TestIterator: CMakeFiles/TestIterator.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fwq/111/bloomfilter_v2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable TestIterator"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TestIterator.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TestIterator.dir/build: TestIterator

.PHONY : CMakeFiles/TestIterator.dir/build

CMakeFiles/TestIterator.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TestIterator.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TestIterator.dir/clean

CMakeFiles/TestIterator.dir/depend:
	cd /home/fwq/111/bloomfilter_v2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fwq/111/bloomfilter_v2 /home/fwq/111/bloomfilter_v2 /home/fwq/111/bloomfilter_v2 /home/fwq/111/bloomfilter_v2 /home/fwq/111/bloomfilter_v2/CMakeFiles/TestIterator.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TestIterator.dir/depend

