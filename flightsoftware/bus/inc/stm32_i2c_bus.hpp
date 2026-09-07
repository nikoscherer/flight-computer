#pragma once

#include "ibus.hpp"

#include "main.h"

class STM32I2CBus : public IBus {
public:
  STM32I2CBus(I2C_HandleTypeDef *hi2c, uint8_t device_address);

  Status writeRegister(const uint8_t reg, uint8_t *data, const size_t size,
                       const uint32_t timeoutMs) override;
  Status writeRegisterDMA(const uint8_t reg, uint8_t *data,
                          const size_t size) override;

  Status readRegister(const uint8_t reg, uint8_t *data, const size_t size,
                      const uint32_t timeoutMs) override;
  Status readRegisterDMA(const uint8_t reg, uint8_t *data,
                         const size_t size) override;

  Status write(const uint8_t *data, const size_t size,
               const uint32_t timeoutMs) override;
  Status writeDMA(const uint8_t *data, const size_t size) override;

  Status read(const uint8_t *data, const size_t size,
              const uint32_t timeoutMs) override;
  Status readDMA(const uint8_t *data, const size_t size) override;

private:
  I2C_HandleTypeDef *hi2c_;
  uint8_t device_address_;
};