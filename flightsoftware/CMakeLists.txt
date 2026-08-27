cmake_minimum_required(VERSION 3.22)
project(FlightSoftwareCore)

add_library(flight_core STATIC
    flight_computer.cpp
)

# add_library(flight_logic STATIC

# )

# target_include_directories(flight_logic PUBLIC
#     .
#     sensors
#     bus
# )

target_link_libraries(flight_core PUBLIC
    stm32cubemx
    # flight_logic
)