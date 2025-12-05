# Specify minimum supported cmake version
cmake_minimum_required(VERSION 3.10)

## hungerland library:
file(GLOB_RECURSE IMGUI_INC_FILES "*.h")
file(GLOB IMGUI_SRC_FILES "*.cpp")
add_library(imgui ${IMGUI_INC_FILES} ${IMGUI_SRC_FILES} backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp)
target_link_libraries(imgui PRIVATE glfw3)

# Specify include directories
target_include_directories(imgui PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
  $<INSTALL_INTERFACE:include/>
)


