project(FlightSoftwareCore CXX)

add_library(flightcore STATIC 
    inc/flightsoftware.hpp
    src/flightsoftware.cpp
)

target_include_directories(flightcore PUBLIC
    inc
    devices/inc
    bus/inc
)

target_sources(flightcore PUBLIC
    devices/src/imu.cpp
    bus/src/stm32_spi_bus.cpp
    bus/src/bus_handler.cpp
)

target_include_directories(flightcore PRIVATE
    ../Core/Inc
    ../Drivers/STM32L4xx_HAL_Driver/Inc
    ../Drivers/CMSIS/Device/ST/STM32L4xx/Include
    ../Drivers/CMSIS/Include

    # FreeRTOS paths (uncomment if using FreeRTOS)
    # ../Middlewares/Third_Party/FreeRTOS/Source/include
    # ../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    # ../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
)