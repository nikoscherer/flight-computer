#pragma once

enum class DeviceStatus { kOk = 0, kInvalidDevice = 1, kError = 2, kNone = 3 };

class IDevice {
   public:
    virtual DeviceStatus initialize() = 0;
};