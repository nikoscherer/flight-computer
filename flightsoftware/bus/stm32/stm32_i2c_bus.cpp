#include "stm32_i2c_bus.hpp"

#include "ibus.hpp"
#include "main.h"
#include "stm32l4xx_hal_def.h"
#include "stm32l4xx_hal_i2c.h"

STM32I2CBus::STM32I2CBus(I2C_HandleTypeDef* hi2c) : hi2c_(hi2c) {}

IBus::CommStatus STM32I2CBus::transmitReceiveDMA(const uint8_t* tx_buffer,
                                                 size_t tx_length,
                                                 uint8_t* rx_buffer,
                                                 size_t rx_length) {
    HAL_StatusTypeDef hal_status =
        HAL_I2C_Mem_Read_DMA(hi2c_, tx_buffer[0], tx_buffer[1],
                             I2C_MEMADD_SIZE_8BIT, rx_buffer, rx_length);
    return parseHALStatus(hal_status);
}

IBus::CommStatus STM32I2CBus::transmitDMA(const uint8_t* tx_buffer,
                                          size_t tx_length) {
    return IBus::CommStatus::kError;
};
IBus::CommStatus STM32I2CBus::receiveDMA(uint8_t* rx_buffer, size_t rx_length) {
    return IBus::CommStatus::kError;
};

IBus::CommStatus STM32I2CBus::transmitReceive(const uint8_t* tx_buffer,
                                              size_t tx_length,
                                              uint8_t* rx_buffer,
                                              size_t rx_length,
                                              uint32_t timeout) {
    HAL_StatusTypeDef hal_status =
        HAL_I2C_Mem_Read(hi2c_, tx_buffer[0], tx_buffer[1],
                         I2C_MEMADD_SIZE_8BIT, rx_buffer, rx_length, timeout);
    return parseHALStatus(hal_status);
}

IBus::CommStatus STM32I2CBus::transmit(uint8_t device_address,
                                       const uint8_t* tx_buffer,
                                       size_t tx_length, uint32_t timeout) {
    HAL_StatusTypeDef hal_status = HAL_I2C_Master_Transmit(
        hi2c_, device_address, const_cast<uint8_t*>(tx_buffer),
        static_cast<uint16_t>(tx_length), timeout);
    return parseHALStatus(hal_status);
}
IBus::CommStatus STM32I2CBus::receive(uint8_t device_address, uint8_t reg,
                                      uint8_t* rx_buffer, size_t rx_length,
                                      uint32_t timeout) {
    HAL_StatusTypeDef hal_status =
        HAL_I2C_Mem_Read(hi2c_, device_address, reg, I2C_MEMADD_SIZE_16BIT,
                         rx_buffer, rx_length, HAL_MAX_DELAY);
    return parseHALStatus(hal_status);
}

IBus::CommStatus STM32I2CBus::getDMAStatus() { return CommStatus::kOk; }

IBus::CommStatus STM32I2CBus::parseHALStatus(HAL_StatusTypeDef hal_status) {
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