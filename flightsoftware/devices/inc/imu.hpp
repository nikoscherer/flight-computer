#pragma once

#include <cstdint>

#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"
#include "spi_handler.hpp"

struct IMUData {
    float x_accel;
    float y_accel;
    float z_accel;
};

class IMU : public IDevice {
   public:
    IMU(IBus* bus, GPIO_TypeDef* cs_gpio_port, uint16_t cs_pin);

    enum class ODR : uint8_t {
        kPowerDown = 0x00,
        k7_5Hz = 0x02,
        k960Hz = 0x09,
        k7_68kHz = 0x0D
    };

    DeviceStatus initialize();

    DeviceStatus read();
    void getReadRequest(SPIBusRequest& bus_request);

    void parseData(IMUData& imu_data);

    uint8_t rx_read_buffer_[10];

   private:
    IBus* bus_;

    GPIO_TypeDef* cs_gpio_port_;
    uint16_t cs_pin_;

    uint8_t tx_read_buffer_[10] = {kRegisterOUTX_L_A_ | 0x80};

    constexpr static uint8_t kDeviceAddress = 0x73;

    constexpr static uint8_t kRegisterWHO_AM_I_ = 0x0F | 0x80,
                             kRegisterCTRL_1_ = 0x10, kRegisterCTRL_3_ = 0x12,
                             kRegisterINT1_CTRL_ = 0x0D,
                             // Read registers
        kRegisterOUTX_L_A_ = 0x28, kRegisterLengthOUT_L_A = 7;
};