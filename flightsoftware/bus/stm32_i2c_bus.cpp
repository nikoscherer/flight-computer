#include "stm32_i2c_bus.hpp"

#include "main.h"

STM32I2CBus::STM32I2CBus(I2C_HandleTypeDef *hi2c, uint8_t device_address)
    : hi2c_(hi2c), device_address_(device_address) {}

STM32I2CBus::Status STM32I2CBus::writeRegister(const uint8_t reg, uint8_t *data,
                                               const size_t size,
                                               const uint32_t timeoutMs) {
  HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
      hi2c_, device_address_, reg, I2C_MEMADD_SIZE_8BIT, data, size, timeoutMs);

  if (status == HAL_ERROR) {
    return Status::kError;
  } else if (status == HAL_BUSY) {
    return Status::kBusy;
  } else if (status == HAL_TIMEOUT) {
    return Status::kTimeout;
  }
  return Status::kOk;
}

STM32I2CBus::Status STM32I2CBus::writeRegisterDMA(const uint8_t reg,
                                                  uint8_t *data,
                                                  const size_t size) {
  HAL_StatusTypeDef status = HAL_I2C_Mem_Write_DMA(
      hi2c_, device_address_, reg, I2C_MEMADD_SIZE_8BIT, data, size);

  if (status == HAL_ERROR) {
    return Status::kError;
  } else if (status == HAL_BUSY) {
    return Status::kBusy;
  }
  return Status::kOk;
}

STM32I2CBus::Status STM32I2CBus::readRegister(const uint8_t reg, uint8_t *data,
                                              const size_t size,
                                              const uint32_t timeoutMs) {
  HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
      hi2c_, device_address_, reg, I2C_MEMADD_SIZE_8BIT, data, size, timeoutMs);

  if (status == HAL_ERROR) {
    return Status::kError;
  } else if (status == HAL_BUSY) {
    return Status::kBusy;
  } else if (status == HAL_TIMEOUT) {
    return Status::kTimeout;
  }
  return Status::kOk;
}

STM32I2CBus::Status STM32I2CBus::readRegisterDMA(const uint8_t reg,
                                                 uint8_t *data,
                                                 const size_t size) {
  HAL_StatusTypeDef status = HAL_I2C_Mem_Read_DMA(
      hi2c_, device_address_, reg, I2C_MEMADD_SIZE_8BIT, data, size);

  if (status == HAL_ERROR) {
    return Status::kError;
  } else if (status == HAL_BUSY) {
    return Status::kBusy;
  }
  return Status::kOk;
}

STM32I2CBus::Status STM32I2CBus::write(const uint8_t *data, const size_t size,
                                       const uint32_t timeoutMs) {
  HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(
      hi2c_, device_address_, (uint8_t *)data, size, timeoutMs);

  if (status == HAL_ERROR) {
    return Status::kError;
  } else if (status == HAL_BUSY) {
    return Status::kBusy;
  } else if (status == HAL_TIMEOUT) {
    return Status::kTimeout;
  }
  return Status::kOk;
}

STM32I2CBus::Status STM32I2CBus::writeDMA(const uint8_t *data,
                                          const size_t size) {
  HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_DMA(hi2c_, device_address_,
                                                         (uint8_t *)data, size);

  if (status == HAL_ERROR) {
    return Status::kError;
  } else if (status == HAL_BUSY) {
    return Status::kBusy;
  }
  return Status::kOk;
}

STM32I2CBus::Status STM32I2CBus::read(const uint8_t *data, const size_t size,
                                      const uint32_t timeoutMs) {
  HAL_StatusTypeDef status = HAL_I2C_Master_Receive(
      hi2c_, device_address_, (uint8_t *)data, size, timeoutMs);

  if (status == HAL_ERROR) {
    return Status::kError;
  } else if (status == HAL_BUSY) {
    return Status::kBusy;
  } else if (status == HAL_TIMEOUT) {
    return Status::kTimeout;
  }
  return Status::kOk;
}

STM32I2CBus::Status STM32I2CBus::readDMA(const uint8_t *data,
                                         const size_t size) {
  HAL_StatusTypeDef status =
      HAL_I2C_Master_Receive_DMA(hi2c_, device_address_, (uint8_t *)data, size);

  if (status == HAL_ERROR) {
    return Status::kError;
  } else if (status == HAL_BUSY) {
    return Status::kBusy;
  }
  return Status::kOk;
}