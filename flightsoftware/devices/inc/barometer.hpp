#pragma once

#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"

class Barometer : public IDevice {
   public:
    Barometer(IBus* bus, GPIO_TypeDef* cs_gpio_port, uint16_t cs_pin);
    DeviceStatus initialize() override;

    constexpr static uint8_t kPROMCount_ = 6;
    uint16_t prom_buffer_[kPROMCount_];

   private:
    struct PROM {
        uint16_t C1;
        uint16_t C2;
        uint16_t C3;
        uint16_t C4;
        uint16_t C5;
        uint16_t C6;
    };

    IBus* bus_;

    GPIO_TypeDef* cs_gpio_port_;
    uint16_t cs_pin_;

    constexpr static uint8_t kCommandReset_ = 0x1E,
                             kCommandReadPROMStart_ = 0xA2, kPROMStep_ = 0x02;
};