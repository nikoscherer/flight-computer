#pragma once

#include "ibus.hpp"
#include "main.h"

class STM32I2CBus : public IBus {
 public:
  STM32I2CBus(I2C_HandleTypeDef& hi2c, uint8_t device_address);

 private:
  I2C_HandleTypeDef& hi2c_;
  uint8_t device_address_;

  HAL_StatusTypeDef writeRegister(const uint8_t reg, uint8_t* data, const size_t size, const uint32_t timeoutMs) override;
  HAL_StatusTypeDef writeRegisterDMA(const uint8_t reg, uint8_t* data, size_t size) override;
  HAL_StatusTypeDef readRegister(const uint8_t reg, uint8_t* data, const size_t size, const uint32_t timeoutMs) override;
  HAL_StatusTypeDef readRegisterDMA(const uint8_t reg, uint8_t* data, size_t size) override;

  HAL_StatusTypeDef write(uint8_t* data, const size_t size, const uint32_t timeoutMs) override;
  HAL_StatusTypeDef writeDMA(uint8_t* data, const size_t size) override;
  HAL_StatusTypeDef read(uint8_t* data, const size_t size, const uint32_t timeoutMs) override;
  HAL_StatusTypeDef readDMA(uint8_t* data, const size_t size) override;
};