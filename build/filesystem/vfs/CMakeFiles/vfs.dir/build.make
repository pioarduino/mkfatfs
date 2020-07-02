# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.11

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "E:\3 micropython\2 micropython-esp32\mkfatfs"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "E:\3 micropython\2 micropython-esp32\mkfatfs\build"

# Include any dependencies generated for this target.
include filesystem/vfs/CMakeFiles/vfs.dir/depend.make

# Include the progress variables for this target.
include filesystem/vfs/CMakeFiles/vfs.dir/progress.make

# Include the compile flags for this target's objects.
include filesystem/vfs/CMakeFiles/vfs.dir/flags.make

filesystem/vfs/CMakeFiles/vfs.dir/user_vfs.c.obj: filesystem/vfs/CMakeFiles/vfs.dir/flags.make
filesystem/vfs/CMakeFiles/vfs.dir/user_vfs.c.obj: filesystem/vfs/CMakeFiles/vfs.dir/includes_C.rsp
filesystem/vfs/CMakeFiles/vfs.dir/user_vfs.c.obj: ../filesystem/vfs/user_vfs.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="E:\3 micropython\2 micropython-esp32\mkfatfs\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object filesystem/vfs/CMakeFiles/vfs.dir/user_vfs.c.obj"
	cd /d "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" && C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\vfs.dir\user_vfs.c.obj   -c "E:\3 micropython\2 micropython-esp32\mkfatfs\filesystem\vfs\user_vfs.c"

filesystem/vfs/CMakeFiles/vfs.dir/user_vfs.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vfs.dir/user_vfs.c.i"
	cd /d "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" && C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "E:\3 micropython\2 micropython-esp32\mkfatfs\filesystem\vfs\user_vfs.c" > CMakeFiles\vfs.dir\user_vfs.c.i

filesystem/vfs/CMakeFiles/vfs.dir/user_vfs.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vfs.dir/user_vfs.c.s"
	cd /d "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" && C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "E:\3 micropython\2 micropython-esp32\mkfatfs\filesystem\vfs\user_vfs.c" -o CMakeFiles\vfs.dir\user_vfs.c.s

filesystem/vfs/CMakeFiles/vfs.dir/vfs.c.obj: filesystem/vfs/CMakeFiles/vfs.dir/flags.make
filesystem/vfs/CMakeFiles/vfs.dir/vfs.c.obj: filesystem/vfs/CMakeFiles/vfs.dir/includes_C.rsp
filesystem/vfs/CMakeFiles/vfs.dir/vfs.c.obj: ../filesystem/vfs/vfs.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="E:\3 micropython\2 micropython-esp32\mkfatfs\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object filesystem/vfs/CMakeFiles/vfs.dir/vfs.c.obj"
	cd /d "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" && C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\vfs.dir\vfs.c.obj   -c "E:\3 micropython\2 micropython-esp32\mkfatfs\filesystem\vfs\vfs.c"

filesystem/vfs/CMakeFiles/vfs.dir/vfs.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vfs.dir/vfs.c.i"
	cd /d "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" && C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "E:\3 micropython\2 micropython-esp32\mkfatfs\filesystem\vfs\vfs.c" > CMakeFiles\vfs.dir\vfs.c.i

filesystem/vfs/CMakeFiles/vfs.dir/vfs.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vfs.dir/vfs.c.s"
	cd /d "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" && C:\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "E:\3 micropython\2 micropython-esp32\mkfatfs\filesystem\vfs\vfs.c" -o CMakeFiles\vfs.dir\vfs.c.s

# Object files for target vfs
vfs_OBJECTS = \
"CMakeFiles/vfs.dir/user_vfs.c.obj" \
"CMakeFiles/vfs.dir/vfs.c.obj"

# External object files for target vfs
vfs_EXTERNAL_OBJECTS =

filesystem/vfs/libvfs.a: filesystem/vfs/CMakeFiles/vfs.dir/user_vfs.c.obj
filesystem/vfs/libvfs.a: filesystem/vfs/CMakeFiles/vfs.dir/vfs.c.obj
filesystem/vfs/libvfs.a: filesystem/vfs/CMakeFiles/vfs.dir/build.make
filesystem/vfs/libvfs.a: filesystem/vfs/CMakeFiles/vfs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="E:\3 micropython\2 micropython-esp32\mkfatfs\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library libvfs.a"
	cd /d "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" && $(CMAKE_COMMAND) -P CMakeFiles\vfs.dir\cmake_clean_target.cmake
	cd /d "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\vfs.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
filesystem/vfs/CMakeFiles/vfs.dir/build: filesystem/vfs/libvfs.a

.PHONY : filesystem/vfs/CMakeFiles/vfs.dir/build

filesystem/vfs/CMakeFiles/vfs.dir/clean:
	cd /d "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" && $(CMAKE_COMMAND) -P CMakeFiles\vfs.dir\cmake_clean.cmake
.PHONY : filesystem/vfs/CMakeFiles/vfs.dir/clean

filesystem/vfs/CMakeFiles/vfs.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "E:\3 micropython\2 micropython-esp32\mkfatfs" "E:\3 micropython\2 micropython-esp32\mkfatfs\filesystem\vfs" "E:\3 micropython\2 micropython-esp32\mkfatfs\build" "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs" "E:\3 micropython\2 micropython-esp32\mkfatfs\build\filesystem\vfs\CMakeFiles\vfs.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : filesystem/vfs/CMakeFiles/vfs.dir/depend
