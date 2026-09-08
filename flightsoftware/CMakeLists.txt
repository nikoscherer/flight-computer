cmake_minimum_required(VERSION 3.22)
project(FlightSoftwareCore)

# Flight Logic Library
add_library(flight_logic STATIC
    containers/src/ring_buffer.cpp # Useless file to allow library creation
)

target_include_directories(flight_logic PUBLIC
    containers/inc
    bus/inc
)

# Flight Core Library
if(NOT BUILD_TESTING)
    add_library(flight_core STATIC
        src/flight_core.cpp
        src/debug.cpp
        bus/src/stm32_i2c_bus.cpp
        sensors/src/adxl343.cpp
    )

    target_include_directories(flight_core PUBLIC
        inc/
        bus/inc/
        sensors/inc/
        containers/inc/
    )

    target_link_libraries(flight_core PUBLIC
        stm32cubemx
        flight_logic
    )
endif()