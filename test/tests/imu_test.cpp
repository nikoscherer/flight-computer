#include "imu.hpp"

#include <gtest/gtest.h>

#include <unordered_map>

#include "ibus.hpp"
#include "mock_spi_bus.hpp"

TEST(IMUTests, InitTest) {
    // std::unordered_map<uint8_t, uint8_t> reg_map = {
    //     {0x0F | 0x80, 0x73},  // WHO_AM_I read
    //     {0x0F, 0x00}          // WHO_AM_I
    // };
    // MockSPIBus spi1_bus(reg_map);
    // IMU imu(CommBusType::kSPI, (int*)0x01, 0x01);

    // BusRequest request;
    // imu.requestRead(request, 100);

    // spi1_bus.transmitReceive(request.tx_buffer, request.rx_buffer,
    //                          request.length, request.timeout);
}