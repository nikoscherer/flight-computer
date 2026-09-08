cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME firmware)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(CXX C ASM)

#---------------------------------------------------------------

# Option for building testing code
option(BUILD_TESTING "Build the testing code" ON)

# Add flight software directory
add_subdirectory(flightsoftware)

# Options for when BUILD_TESTING is ON/OFF
if(NOT BUILD_TESTING)
    add_executable(${CMAKE_PROJECT_NAME}) # Create an executable object type only for firmware builds
    add_subdirectory(cmake/stm32cubemx) # Add STM32CubeMX generated sources
    target_link_libraries(${CMAKE_PROJECT_NAME} # Add STM32CubeMX generated library and flight_core
        stm32cubemx
        flight_core
    )
endif()

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

#---------------------------------------------------------------

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)