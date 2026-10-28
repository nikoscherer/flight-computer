#pragma once

#include "ibus.hpp"
#include "isensor.hpp"
#include "main.h"

struct IMUData {
    float x_accel;
    float y_accel;
    float z_accel;
};

class IMU : public ISensor {
   public:
    IMU(CommBusType bus_type);

    void requestRead(BusRequest& request);

    void getAccel(IMUData& imu_data);

    uint8_t rx_read_buffer_[10];
    uint8_t tx_read_buffer_[10] = {kRegisterOUTX_L_A_ | 0x80};

   private:
    CommBusType bus_type_;

    constexpr static uint8_t kRegisterOUTX_L_A_ = 0x28,
                             kRegisterLengthOUT_L_A = 7;
};