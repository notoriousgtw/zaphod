cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(zaphod-engine)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

find_package(glm CONFIG REQUIRED)
find_package(Vulkan)
include_directories(${Vulkan_INCLUDE_DIRS})
#add_compile_definitions(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

# Recurse all cpp sources in new folder layout
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/**/*.cpp"
    "src/**/**/*.cpp"
)

# Debug: Print the found sources
message(STATUS "Found engine sources: ${SOURCES}")

# Ensure we have source files
if(NOT SOURCES)
    message(FATAL_ERROR "No source files found! Check your directory structure.")
endif()


add_library(zaphod-engine STATIC ${SOURCES})

# Set consistent output directories
set_target_properties(zaphod-engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_definitions(zaphod-engine PRIVATE
    GLFW_STATIC
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)

target_include_directories(zaphod-engine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/generated>
    $<INSTALL_INTERFACE:include>
)

message(${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.frag")
message(${SHADERS})
set(GENERATED_HEADERS "")
set(SPIRV_FILES "")


# Shader compilation (if you have shaders)
file(GLOB SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.frag")
if(SHADERS)
    set(GENERATED_HEADERS "")
    set(SPIRV_FILES "")

    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/src/shaders/${SHADER_NAME}.spv")
        set(HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/generated/${SHADER_NAME}.spv.h")
    
        add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shaders
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV}
            DEPENDS ${SHADER}
            COMMENT "Compiling ${SHADER_NAME} to SPIR-V"
            VERBATIM
        )

        add_custom_command(
            OUTPUT "${HEADER}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/include/generated"
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/spv_to_header.py" "${SPIRV}" "${HEADER}"
            DEPENDS "${SPIRV}"
            COMMENT "Converting ${SPIRV} to header in source tree"
            VERBATIM
        )

        list(APPEND GENERATED_HEADERS ${HEADER})
        list(APPEND SPIRV_FILES ${SPIRV})
    endforeach()

    add_custom_target(ShaderSpvFiles DEPENDS ${SPIRV_FILES})
    add_custom_target(ShaderHeaders DEPENDS ${GENERATED_HEADERS})
    add_dependencies(ShaderHeaders ShaderSpvFiles)
    add_dependencies(zaphod-engine ShaderHeaders)

    # Ensure generated headers are part of target sources for IDEs
    if(GENERATED_HEADERS)
        target_sources(zaphod-engine PRIVATE ${GENERATED_HEADERS})
    endif()
endif()

target_link_libraries(zaphod-engine PUBLIC Vulkan::Vulkan glm::glm glfw)

install(TARGETS zaphod-engine EXPORT zaphod-engineTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
    )

#install(EXPORT zaphod-engineTargets
#        FILE zaphod-engineConfig.cmake
#        NAMESPACE zaphod-engine::
#        DESTINATION lib/cmake/zaphod-engine
#    )

# Install generated shader headers
#install(
#    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/generated/"
#    DESTINATION include/generated
#    FILES_MATCHING PATTERN "*.spv.h"
#)

