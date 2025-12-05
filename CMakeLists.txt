cmake_minimum_required(VERSION 3.23)
project(EmscriptenMapEditor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_WEBGL2=1 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -s STACK_SIZE=15MB -s INITIAL_MEMORY=512MB")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']")

add_subdirectory("ext")
add_subdirectory("engine")
add_subdirectory("editor")

include_directories("ext/stb-master")