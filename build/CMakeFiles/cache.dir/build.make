# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator\build

# Include any dependencies generated for this target.
include CMakeFiles/cache.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cache.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cache.dir/flags.make

CMakeFiles/cache.dir/cacheSim.cpp.obj: CMakeFiles/cache.dir/flags.make
CMakeFiles/cache.dir/cacheSim.cpp.obj: ../cacheSim.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cache.dir/cacheSim.cpp.obj"
	C:\TDM-GCC-64\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\cache.dir\cacheSim.cpp.obj -c C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator\cacheSim.cpp

CMakeFiles/cache.dir/cacheSim.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cache.dir/cacheSim.cpp.i"
	C:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator\cacheSim.cpp > CMakeFiles\cache.dir\cacheSim.cpp.i

CMakeFiles/cache.dir/cacheSim.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cache.dir/cacheSim.cpp.s"
	C:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator\cacheSim.cpp -o CMakeFiles\cache.dir\cacheSim.cpp.s

# Object files for target cache
cache_OBJECTS = \
"CMakeFiles/cache.dir/cacheSim.cpp.obj"

# External object files for target cache
cache_EXTERNAL_OBJECTS =

cache.exe: CMakeFiles/cache.dir/cacheSim.cpp.obj
cache.exe: CMakeFiles/cache.dir/build.make
cache.exe: CMakeFiles/cache.dir/linklibs.rsp
cache.exe: CMakeFiles/cache.dir/objects1.rsp
cache.exe: CMakeFiles/cache.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable cache.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\cache.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cache.dir/build: cache.exe

.PHONY : CMakeFiles/cache.dir/build

CMakeFiles/cache.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\cache.dir\cmake_clean.cmake
.PHONY : CMakeFiles/cache.dir/clean

CMakeFiles/cache.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator\build C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator\build C:\Users\napoel92\Desktop\Old_Courses\comSTR\projects\new\hw2\Cache-Memory-Simulator\build\CMakeFiles\cache.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cache.dir/depend

