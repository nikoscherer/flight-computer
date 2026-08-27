#pragma once

#include <stddef.h>
#include <stdint.h>

class IBus {
public:
  virtual ~IBus() = default;

  enum class Status : uint8_t { kOk = 0, kBusy = 1, kTimeout = 2, kError = 3 };

  virtual Status writeRegister(const uint8_t reg, uint8_t *data,
                               const size_t size, const uint32_t timeoutMs) = 0;
  virtual Status writeRegisterDMA(const uint8_t reg, uint8_t *data,
                                  const size_t size) = 0;

  virtual Status readRegister(const uint8_t reg, uint8_t *data,
                              const size_t size, const uint32_t timeoutMs) = 0;
  virtual Status readRegisterDMA(const uint8_t reg, uint8_t *data,
                                 const size_t size) = 0;

  virtual Status write(const uint8_t *data, const size_t size,
                       const uint32_t timeoutMs) = 0;
  virtual Status writeDMA(const uint8_t *data, const size_t size) = 0;

  virtual Status read(const uint8_t *data, const size_t size,
                      const uint32_t timeoutMs) = 0;
  virtual Status readDMA(const uint8_t *data, const size_t size) = 0;
};