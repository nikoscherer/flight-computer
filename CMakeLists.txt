cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

option(BUILD_TESTS "Build unit tests on the host" OFF)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME flight-computer)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME} CXX C ASM)
message("Build type: " ${CMAKE_BUILD_TYPE})

if (NOT BUILD_TESTS)
    add_compile_definitions(STM32L412xx)
endif()

# Add project libraries
add_subdirectory(flightsoftware)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

if (BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(test)
else()
    # Create an executable object type
    add_executable(${CMAKE_PROJECT_NAME})
    
    # Add STM32CubeMX generated sources
    add_subdirectory(cmake/stm32cubemx)

    target_link_libraries(${CMAKE_PROJECT_NAME}
        stm32cubemx
        flightcore
    )
endif()