#pragma once

#include "main.h"

class IBus {
public:
  virtual ~IBus() = default;

  virtual HAL_StatusTypeDef writeRegister(const uint8_t reg, uint8_t* data, const size_t size, const uint32_t timeoutMs);
  virtual HAL_StatusTypeDef writeRegisterDMA(const uint8_t reg, uint8_t* data, size_t size);
  virtual HAL_StatusTypeDef readRegister(const uint8_t reg, uint8_t* data, const size_t size, const uint32_t timeoutMs);
  virtual HAL_StatusTypeDef readRegisterDMA(const uint8_t reg, uint8_t* data, size_t size);

  virtual HAL_StatusTypeDef write(uint8_t* data, const size_t size, const uint32_t timeoutMs);
  virtual HAL_StatusTypeDef writeDMA(uint8_t* data, const size_t size);
  virtual HAL_StatusTypeDef read(uint8_t* data, const size_t size, const uint32_t timeoutMs);
  virtual HAL_StatusTypeDef readDMA(uint8_t* data, const size_t size);
};