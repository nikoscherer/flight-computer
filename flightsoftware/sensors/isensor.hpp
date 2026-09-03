#pragma once

#include <stdint.h>

class ISensor {
public:
  virtual ~ISensor() = default;

  enum class Status : uint8_t {
    kOk = 0,
    kBusy = 1,
    kError = 2,
    kInvalidID = 3
  };

  virtual Status init() = 0;
  virtual Status reset() = 0;
};