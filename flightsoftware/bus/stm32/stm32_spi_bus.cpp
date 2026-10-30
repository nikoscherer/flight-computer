#include "stm32_spi_bus.hpp"

#include "ibus.hpp"
#include "main.h"
#include "stm32l4xx_hal_def.h"
#include "stm32l4xx_hal_spi.h"

STM32SPIBus::STM32SPIBus(SPI_HandleTypeDef* hspi) : hspi_(hspi) {}

IBus::CommStatus STM32SPIBus::transmitReceiveDMA(uint8_t* tx_buffer,
                                                 uint8_t* rx_buffer,
                                                 size_t length) {
    HAL_StatusTypeDef hal_status =
        HAL_SPI_TransmitReceive_DMA(hspi_, tx_buffer, rx_buffer, length);
    return parseHALStatus(hal_status);
}

IBus::CommStatus STM32SPIBus::transmitDMA(uint8_t* tx_buffer, size_t length) {
    HAL_StatusTypeDef hal_status =
        HAL_SPI_Transmit_DMA(hspi_, tx_buffer, length);
    return parseHALStatus(hal_status);
}

IBus::CommStatus STM32SPIBus::receiveDMA(uint8_t* rx_buffer, size_t length) {
    HAL_StatusTypeDef hal_status =
        HAL_SPI_Receive_DMA(hspi_, rx_buffer, length);
    return parseHALStatus(hal_status);
}

IBus::CommStatus STM32SPIBus::transmitReceive(uint8_t* tx_buffer,
                                              uint8_t* rx_buffer, size_t length,
                                              uint32_t timeout) {
    HAL_StatusTypeDef hal_status =
        HAL_SPI_TransmitReceive(hspi_, tx_buffer, rx_buffer, length, timeout);
    return parseHALStatus(hal_status);
}

IBus::CommStatus STM32SPIBus::transmit(uint8_t* tx_buffer, size_t length,
                                       uint32_t timeout) {
    HAL_StatusTypeDef hal_status =
        HAL_SPI_Transmit(hspi_, tx_buffer, length, timeout);
    return parseHALStatus(hal_status);
}

IBus::CommStatus STM32SPIBus::receive(uint8_t* rx_buffer, size_t length,
                                      uint32_t timeout) {
    HAL_StatusTypeDef hal_status =
        HAL_SPI_Receive(hspi_, rx_buffer, length, timeout);
    return parseHALStatus(hal_status);
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

IBus::CommStatus STM32SPIBus::getDMAStatus() {
    HAL_SPI_StateTypeDef spi_status = hspi_->State;

    switch (spi_status) {
        case HAL_SPI_STATE_READY:
            return IBus::CommStatus::kOk;
        case HAL_SPI_STATE_BUSY:
        case HAL_SPI_STATE_BUSY_TX_RX:
        case HAL_SPI_STATE_BUSY_TX:
        case HAL_SPI_STATE_BUSY_RX:
            return IBus::CommStatus::kBusy;
        case HAL_SPI_STATE_ERROR:
            return IBus::CommStatus::kError;
        default:  // unknown
            return CommStatus::kError;
    }
}