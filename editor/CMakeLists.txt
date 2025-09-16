cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(zaphod-editor)

# Collect all editor source files
file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Create the editor executable
add_executable(zaphod-editor ${EDITOR_SOURCES})

# Link the engine static library
target_link_libraries(zaphod-editor PRIVATE zaphod-engine)

# Include directories for editor-specific headers
target_include_directories(zaphod-editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set output directory (optional)
set_target_properties(zaphod-editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
