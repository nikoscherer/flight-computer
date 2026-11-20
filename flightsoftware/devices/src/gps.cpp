#include "gps.hpp"

#include <cmath>
#include <cstddef>

#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"
#include "stm32l4xx_hal_def.h"

GPS::GPS(IBus* bus) : bus_(bus) {}

DeviceStatus GPS::initialize() {
    // LSB first
    uint8_t config[] = {
        0xB5,
        0x62,  // Header
        0x06,
        0x8A,  // Class, ID
        0x22,
        0x00,  // Length (MUST CHANGE WHEN ADDING CONFIGURATIONS)
        0x00,  // Version
        0x01,  // RAM
        0x00,
        0x00,  // Reserved
        // Configuration Data
        0x01, 0x00, 0x71, 0x10,
        0x01,  // (0x10710001 0x01 -> enable UBX input for I2C)
        0x02, 0x00, 0x71, 0x10,
        0x00,  // (0x10710002 0x00 -> disable NMEA input for I2C)
        0x01, 0x00, 0x72, 0x10,
        0x01,  // (0x10710001 0x01 -> enable UBX output for I2C)
        0x02, 0x00, 0x72, 0x10,
        0x00,  // (0x10710002 0x00 -> disable NMEA output for I2C)
        0x06, 0x00, 0x91, 0x20,
        0x01,  // (0x20910006 0x01 -> enable UBX-NAV-PVT for I2C )
        0x01, 0x00, 0xa2, 0x10,
        0x01,  // (0x10a20001 0x01 -> enable TX-READY (data ready interrupt))
        0x00,
        0x00  // Placeholders for checksum
    };

    uint8_t ck_a = 0;
    uint8_t ck_b = 0;

    for (size_t i = 2; i < sizeof(config) - 2; ++i) {
        ck_a = ck_a + config[i];
        ck_b = ck_b + ck_a;
    }

    config[sizeof(config) - 2] = ck_a;
    config[sizeof(config) - 1] = ck_b;

    IBus::CommStatus status =
        bus_->transmit(kDeviceAddress, config, sizeof(config), HAL_MAX_DELAY);

    return status == IBus::CommStatus::kOk ? DeviceStatus::kOk
                                           : DeviceStatus::kError;
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

// GPS IS LITTLE ENDIAN (LSB FIRST)
// Need to use valid, fixType, flags, flags2, and flags3 to verify
bool GPS::parseData() {
    // UBX-NAV-PVT
    if (rx_buffer_[0] == kUBXHeader_[0] && rx_buffer_[1] == kUBXHeader_[1] &&
        rx_buffer_[2] == kUBX_NAV_PVT_Class_ &&
        rx_buffer_[3] == kUBX_NAV_PVT_ID_) {
        // Length of payload should be 92 bytes
        if ((static_cast<uint16_t>(rx_buffer_[5]) << 8 | rx_buffer_[4]) != 92) {
            return false;
        }

        // iTOW is [6] -> [9]
        uint32_t iTOW =
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 3])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 2])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 1])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 0];

        // year is [10] -> [11]
        uint16_t year =
            (static_cast<uint16_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 5])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 4];
        // month is [12]
        uint8_t month = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 6];
        // day is [13]
        uint8_t day = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 7];
        // hour is [14]
        uint8_t hour = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 8];
        // min is [15]
        uint8_t min = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 9];
        // sec is [16]
        uint8_t sec = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 10];

        // valid is [17]
        uint8_t valid = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ +
                                   11];  // This is a bitmask of 0x0F

        // tAcc is [18] -> [21]
        uint32_t tAcc =
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 15])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 14])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 13])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 12];

        // nano is [22] -> [25]
        int32_t nano = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 19])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 18])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 17])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 16]);

        // fixType is [26]
        uint8_t fixType = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 20];

        // flags is [27]
        uint8_t flags = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 21];

        // flags2 is [28]
        uint8_t flags2 = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 22];

        // numSV is [29]
        uint8_t numSV = rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 23];

        // lon (x1e-7 deg) is [30] -> [33]
        int32_t lon = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 27])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 26])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 25])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 24]);

        // lat (xe-7 deg) is [34] -> [37]
        int32_t lat = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 31])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 30])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 29])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 28]);

        // height is [38] -> [41]
        int32_t height = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 35])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 34])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 33])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 32]);

        // hMSL is [42] -> [45]
        int32_t hMSL = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 39])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 38])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 37])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 36]);

        // hAcc is [46] -> [49]
        uint32_t hAcc =
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 43])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 42])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 41])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 40];

        // vAcc is [50] -> [53]
        uint32_t vAcc =
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 47])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 46])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 45])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 44];

        // velN is [54] -> [57]
        int32_t velN = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 51])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 50])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 49])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 48]);

        // velN is [58] -> [61]
        int32_t velE = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 55])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 54])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 53])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 52]);

        // velD is [62] -> [65]
        int32_t velD = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 59])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 58])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 57])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 56]);

        // gSpeed is [66] -> [69]
        int32_t gSpeed = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 63])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 62])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 61])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 60]);

        // headMot (xe-5) is [70] -> [73]
        int32_t headMot = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 67])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 66])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 65])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 64]);

        // sAcc is [74] -> [77]
        uint32_t sAcc =
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 71])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 70])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 69])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 68];

        // headAcc (xe-5) is [78] -> [81]
        uint32_t headAcc =
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 75])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 74])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 73])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 72];

        // pDOP (x0.01) is [82] -> [83]
        uint16_t pDOP =
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 77])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 76];

        // flags3 is [84] -> [85]
        uint16_t flags3 =
            (static_cast<uint16_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 79])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 78];

        // [86] -> [89] is reserved

        // headVeh (xe-5) is [90] -> [93]
        int32_t headVeh = std::bit_cast<int32_t>(
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 87])
             << 24) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 86])
             << 16) |
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 85])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 84]);

        // magDec (xe-2) is [95] -> [98]
        int16_t magDec = std::bit_cast<int16_t>(static_cast<uint16_t>(
            (static_cast<uint16_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 89])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 88]));

        // magAcc (xe-2) is [99] -> [100]
        uint16_t magAcc =
            (static_cast<uint32_t>(rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 91])
             << 8) |
            rx_buffer_[kUBX_NAV_PVT_Payload_Start_ + 90];

        // Actual values
        float latitude = static_cast<float>(lat) * pow(10.0, -7.0);
        float longitude = static_cast<float>(lon) * pow(10.0, -7.0);

        return true;
    }

    return false;
}

uint16_t GPS::getAvailableBytes() {
    uint8_t tx_buffer[2] = {kDeviceAddress, kRegisterNumBytesAvailableHigh_};
    uint8_t rx_buffer[2];

    bus_->transmitReceive(tx_buffer, sizeof(tx_buffer), rx_buffer,
                          sizeof(rx_buffer), HAL_MAX_DELAY);

    return static_cast<uint16_t>(rx_buffer[0]) << 8 |
           static_cast<uint16_t>(rx_buffer[1]);
}