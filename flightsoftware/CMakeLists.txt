# Flight Core
# include/sources that use the STM32 framework.
add_library(flightcore STATIC 
    src/flightsoftware.cpp
    bus/stm32/stm32_spi_bus.cpp
)

target_include_directories(flightcore PUBLIC
    inc
    bus/stm32
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

# Flight Logic
# include/sources that don't use the STM32 framework
add_library(flightlogic STATIC
    bus/src/bus_handler.cpp
    devices/src/imu.cpp
)

target_include_directories(flightlogic PUBLIC
    bus/inc
    devices/inc
)

target_link_libraries(flightcore PUBLIC flightlogic)