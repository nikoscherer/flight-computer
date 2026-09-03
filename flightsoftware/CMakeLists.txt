cmake_minimum_required(VERSION 3.22)
project(FlightSoftwareCore)

# Flight Logic Library
add_library(flight_logic STATIC
    src/debug.cpp inc/debug.hpp
)

target_include_directories(flight_logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_sources(flight_logic PRIVATE
    src/debug.cpp
)

# Flight Core Library
add_library(flight_core STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_include_directories(flight_core PUBLIC
    inc/
    bus/
)

target_sources(flight_core PRIVATE
    src/flight_core.cpp
    bus/stm32_i2c_bus.cpp
)

target_link_libraries(flight_core PUBLIC
    flight_logic
    stm32cubemx
)