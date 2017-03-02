cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Release)

include(ExternalProject)

set(DESTDIR ${CMAKE_CURRENT_LIST_DIR}/install)

add_subdirectory(LuaJIT)

include_directories(${CMAKE_BINARY_DIR}/LuaJIT/LuaJIT/src)
include_directories(${OCAMLLIB})

link_directories(${OCAMLLIB})
link_directories(${CMAKE_BINARY_DIR}/LuaJIT/LuaJIT/src)

if(APPLE)
set(CMAKE_EXE_LINKER_FLAGS -pagezero_size 10000 -image_base 100000000)
endif(APPLE)

if(UNIX AND NOT APPLE)
	set(LIBRARIES m dl)
endif(UNIX AND NOT APPLE)

add_subdirectory(AudioEngine)
