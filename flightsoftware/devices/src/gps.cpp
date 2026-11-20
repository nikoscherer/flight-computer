#include "gps.hpp"

#include <cstddef>

#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"
#include "stm32l4xx_hal_def.h"

GPS::GPS(IBus* bus) : bus_(bus) {}

DeviceStatus GPS::initialize() {
    uint8_t nmea_off[] = {
        0xB5,
        0x62,  // Header
        0x06,
        0x8A,  // Class, ID
        0x09,
        0x00,  // Length
        0x00,  // Version
        0x01,  // RAM
        0x00,
        0x00,  // Reserved
        0x02, 0x00, 0x72,
        0x10  // Configuration Data (0x10720002 -> disable NMEA)
    };

    bus_->transmit(kDeviceAddress, nmea_off, sizeof(nmea_off), HAL_MAX_DELAY);

    return DeviceStatus::kOk;
}

bool GPS::read() {
    uint16_t byte_count = getAvailableBytes();

    if (byte_count == 0) {
        return false;
    }

    uint8_t tx_buffer[2] = {kDeviceAddress, 0xFF};
    bus_->transmitReceive(tx_buffer, sizeof(tx_buffer), rx_buffer_, byte_count,
                          HAL_MAX_DELAY);

    return true;
}

uint16_t GPS::getAvailableBytes() {
    uint8_t tx_buffer[2] = {kDeviceAddress, kRegisterNumBytesAvailableHigh_};
    uint8_t rx_buffer[2];

    bus_->transmitReceive(tx_buffer, 2, rx_buffer, sizeof(rx_buffer),
                          HAL_MAX_DELAY);

    return static_cast<uint16_t>(rx_buffer[0]) << 8 |
           static_cast<uint16_t>(rx_buffer[1]);
}