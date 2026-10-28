#include "stm32_spi_bus.hpp"

#include "ibus.hpp"
#include "main.h"

STM32SPIBus::STM32SPIBus(SPI_HandleTypeDef* hspi) : hspi_(hspi) {}

IBus::CommStatus STM32SPIBus::transmitReceive(uint8_t* tx_buffer,
                                              uint8_t* rx_buffer, size_t length,
                                              uint32_t timeout) {
    HAL_StatusTypeDef hal_status =
        HAL_SPI_TransmitReceive(hspi_, tx_buffer, rx_buffer, length, timeout);
    return parseHALStatus(hal_status);
}

IBus::CommStatus STM32SPIBus::transmit(uint8_t* tx_buffer, size_t length,
                                       uint32_t timeout) {
    return IBus::CommStatus::kError;
}

IBus::CommStatus STM32SPIBus::receive(uint8_t* rx_buffer, size_t length,
                                      uint32_t timeout) {
    return IBus::CommStatus::kError;
}

IBus::CommStatus STM32SPIBus::parseHALStatus(HAL_StatusTypeDef hal_status) {
    switch (hal_status) {
        case HAL_OK:
            return CommStatus::kOk;
        case HAL_TIMEOUT:
            return CommStatus::kTimeout;
        case HAL_BUSY:
            return CommStatus::kBusy;
        case HAL_ERROR:
            return CommStatus::kError;
    }

    return CommStatus::kError;
}