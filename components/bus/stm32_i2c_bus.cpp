#include "stm32_i2c_bus.hpp"

#include "main.h"

STM32I2CBus::STM32I2CBus(I2C_HandleTypeDef &hi2c, uint8_t device_address) 
    : hi2c_(hi2c), device_address_(device_address) {

}

HAL_StatusTypeDef STM32I2CBus::writeRegister(const uint8_t reg, uint8_t* data, const size_t size, const uint32_t timeoutMs) {
    return HAL_I2C_Mem_Write(
        &hi2c_, 
        device_address_, 
        reg, 
        I2C_MEMADD_SIZE_8BIT, 
        data, 
        size, 
        timeoutMs);
}

HAL_StatusTypeDef STM32I2CBus::writeRegisterDMA(const uint8_t reg, uint8_t* data, size_t size) {
    return HAL_I2C_Mem_Write_DMA(
        &hi2c_, 
        device_address_, 
        reg, 
        I2C_MEMADD_SIZE_8BIT, 
        data, 
        size);
}

HAL_StatusTypeDef STM32I2CBus::readRegister(const uint8_t reg, uint8_t* data, const size_t size, const uint32_t timeoutMs) {
    return HAL_I2C_Mem_Read(
        &hi2c_,
        device_address_, 
        reg, 
        I2C_MEMADD_SIZE_8BIT, 
        data, 
        size, 
        timeoutMs);
}

HAL_StatusTypeDef STM32I2CBus::readRegisterDMA(const uint8_t reg, uint8_t* data, size_t size) {
    return HAL_I2C_Mem_Read_DMA(
        &hi2c_, 
        device_address_, 
        reg, 
        I2C_MEMADD_SIZE_8BIT, 
        data, 
        size);
}

HAL_StatusTypeDef STM32I2CBus::write(uint8_t* data, const size_t size, const uint32_t timeoutMs) {
    return HAL_I2C_Master_Transmit(
        &hi2c_, 
        device_address_, 
        data, 
        size, 
        timeoutMs);
}

HAL_StatusTypeDef STM32I2CBus::writeDMA(uint8_t* data, const size_t size) {
    return HAL_I2C_Master_Transmit_DMA(&hi2c_, device_address_, data, size);
}

HAL_StatusTypeDef STM32I2CBus::read(uint8_t* data, const size_t size, const uint32_t timeoutMs) {
    return HAL_I2C_Master_Receive(
        &hi2c_, 
        device_address_,
        data, 
        size, 
        timeoutMs);
}

HAL_StatusTypeDef STM32I2CBus::readDMA(uint8_t* data, const size_t size) {
    return HAL_I2C_Master_Receive_DMA(&hi2c_, device_address_, data, size);
}