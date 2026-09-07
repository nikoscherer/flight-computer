cmake_minimum_required(VERSION 3.22)
project(FlightSoftwareCore)

# Flight Logic Library
# add_library(flight_logic STATIC
    
# )

# target_include_directories(flight_logic PUBLIC
#     bus/inc/
# )

# target_sources(flight_logic PRIVATE

# )

# Flight Core Library
add_library(flight_core STATIC
    inc/flight_core.hpp
    src/flight_core.cpp
)

target_include_directories(flight_core PUBLIC
    inc/
    bus/inc/
    sensors/inc/
)

target_sources(flight_core PRIVATE
    src/flight_core.cpp
    src/debug.cpp
    bus/src/stm32_i2c_bus.cpp
    sensors/src/adxl343.cpp
)

target_link_libraries(flight_core PUBLIC
    # flight_logic
    stm32cubemx
)