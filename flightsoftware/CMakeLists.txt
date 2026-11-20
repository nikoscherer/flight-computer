# Flight Core
# include/sources that use the STM32 framework.
if (NOT BUILD_TESTS)
    add_library(flightcore OBJECT # Needs to include EVERYTHING, interrupts.cpp gets dropped.
        src/flightsoftware.cpp
        src/hardware.cpp
        src/interrupts.cpp

        # Bus
        bus/stm32/stm32_spi_bus.cpp
        bus/stm32/stm32_i2c_bus.cpp
        bus/src/bus_handler.cpp
        bus/src/spi_handler.cpp

        # Devices
        devices/imu/imu.cpp
        devices/src/barometer.cpp
        devices/src/gps.cpp
    )

    target_include_directories(flightcore PUBLIC
        inc
        bus/inc
        bus/stm32
        devices/inc
        devices/imu
    )

    target_include_directories(flightcore PRIVATE
        ../Core/Inc
        ../Drivers/STM32L4xx_HAL_Driver/Inc
        ../Drivers/CMSIS/Device/ST/STM32L4xx/Include
        ../Drivers/CMSIS/Include

        # FreeRTOS paths (uncomment if using FreeRTOS)
        ../Middlewares/Third_Party/FreeRTOS/Source/include
        ../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
        ../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    )

    # target_link_libraries(flightcore PUBLIC flightlogic)
endif()

# Flight Logic
# include/sources that don't use the STM32 framework
# add_library(flightlogic STATIC
#     # bus/src/bus_handler.cpp Should be here
#     bus/src/placeholder.cpp
# )

# target_include_directories(flightlogic PUBLIC
    
# )