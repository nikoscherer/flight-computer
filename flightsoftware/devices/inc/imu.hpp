#pragma once

#include <cstdint>

#include "ibus.hpp"
#include "isensor.hpp"

struct IMUData {
    float x_accel;
    float y_accel;
    float z_accel;
};

class IMU : public ISensor {
   public:
    IMU(CommBusType bus_type, int* cs_gpio_port, uint16_t cs_pin);

    void requestRead(BusRequest& request, uint32_t timeout);

    void getAccel(IMUData& imu_data);

    uint8_t rx_read_buffer_[10];
    uint8_t tx_read_buffer_[10] = {kRegisterOUTX_L_A_ | 0x80};

   private:
    CommBusType bus_type_;

    int* cs_gpio_port_;
    uint16_t cs_pin_;

    constexpr static uint8_t kRegisterOUTX_L_A_ = 0x28,
                             kRegisterLengthOUT_L_A = 7;
};