#pragma once

#include "main.h"

struct SensorStatus : uint8_t {
    kOk = 0,
    kErrBus = 1,
    kErrInvalidId = 2;
};