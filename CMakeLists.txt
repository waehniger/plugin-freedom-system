cmake_minimum_required(VERSION 3.22)

# Set deployment target before project
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS deployment version")

project(JUCEPlugins VERSION 1.0.0)

# Find JUCE - check multiple standard locations
set(JUCE_PATHS
    "$ENV{HOME}/JUCE"
    "/Applications/JUCE"
    "$ENV{HOME}/Developer/JUCE"
    "/usr/local/JUCE"
)

set(JUCE_PATH "")

foreach(PATH ${JUCE_PATHS})
    if(EXISTS "${PATH}/modules/juce_core/juce_core.h")
        set(JUCE_PATH "${PATH}")
        break()
    endif()
endforeach()

if(JUCE_PATH STREQUAL "")
    message(FATAL_ERROR "JUCE not found. Please install JUCE to one of these locations:\n"
                        "  - ~/JUCE\n"
                        "  - /Applications/JUCE\n"
                        "  - ~/Developer/JUCE\n"
                        "  - /usr/local/JUCE\n"
                        "\n"
                        "Or set JUCE_PATH environment variable:\n"
                        "  export JUCE_PATH=/path/to/JUCE")
endif()

message(STATUS "Using JUCE from: ${JUCE_PATH}")

# Add JUCE once at root
add_subdirectory(${JUCE_PATH} JUCE)

# Auto-discover plugins
file(GLOB PLUGIN_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/plugins/*")
foreach(PLUGIN_DIR ${PLUGIN_DIRS})
    if(IS_DIRECTORY ${PLUGIN_DIR} AND EXISTS "${PLUGIN_DIR}/CMakeLists.txt")
        add_subdirectory(${PLUGIN_DIR})
    endif()
endforeach()
