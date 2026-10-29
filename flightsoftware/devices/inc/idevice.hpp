#pragma once

enum class DeviceStatus {
    kOk = 0,
    kInvalidDevice = 1,
};

class IDevice {
   public:
    enum class CommStatus { kOk = 0, kError = 1 };
};