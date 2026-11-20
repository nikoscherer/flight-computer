#pragma once

#include "ibus.hpp"
#include "idevice.hpp"

class GPS : public IDevice {
   public:
    GPS(IBus* bus);

    DeviceStatus initialize();

    bool read();

    bool parseData();

    uint16_t getAvailableBytes();

    uint8_t rx_buffer_[1024] = {};

   private:
    IBus* bus_;

    constexpr static uint8_t kDeviceAddress = 0x42
                                              << 1;  // Shifted 1 bit for HAL

    constexpr static uint8_t kRegisterNumBytesAvailableHigh_ = 0xFD;

    // UBX
    constexpr static uint8_t kUBXHeader_[2] = {0xb5, 0x62};
    constexpr static uint8_t kUBX_NAV_PVT_Class_ = 0x01;
    constexpr static uint8_t kUBX_NAV_PVT_ID_ = 0x07;
    constexpr static uint8_t kUBX_NAV_PVT_Payload_Start_ = 0x06;
};