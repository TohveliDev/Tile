set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ENGINE_INC_FILES "include/*.h")
file(GLOB_RECURSE ENGINE_SRC_FILES "src/*.cpp")

add_library(engine ${ENGINE_INC_FILES} ${ENGINE_SRC_FILES})

target_link_libraries(engine PUBLIC glfw3 imgui glm)

if(EMSCRIPTEN)
  target_link_libraries(engine PRIVATE embind)
endif()

target_include_directories(engine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(engine PUBLIC "${PROJECT_SOURCE_DIR}/ext/stb-master")