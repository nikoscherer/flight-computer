#pragma once

#include "ibus.hpp"
#include "idevice.hpp"

class GPS : public IDevice {
   public:
    GPS(IBus* bus);

    DeviceStatus initialize();

    bool read();

    uint16_t getAvailableBytes();

    uint8_t rx_buffer_[1] = {};

   private:
    IBus* bus_;

    constexpr static uint8_t kDeviceAddress = 0x84;

    constexpr static uint8_t kRegisterNumBytesAvailableHigh_ = 0xFD;
};