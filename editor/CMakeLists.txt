project(Tile)

set(HEADERS
)

set(SOURCES
)

add_executable (tile main_editor.cpp)
target_link_libraries(tile engine)

set_target_properties(tile PROPERTIES
    OUTPUT_NAME "index"
    SUFFIX ".html"
)

target_link_options(tile PRIVATE
    --shell-file "${CMAKE_CURRENT_SOURCE_DIR}/template.html"
    --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/assets@assets"
)