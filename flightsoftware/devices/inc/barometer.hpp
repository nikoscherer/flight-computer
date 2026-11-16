#pragma once

#include "bus_handler.hpp"
#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"

struct BarometerData {
    float pressure;
    float temperature;
};

class Barometer : public IDevice {
   public:
    Barometer(IBus* bus, GPIO_TypeDef* cs_gpio_port, uint16_t cs_pin,
              SPIDeviceID dev_id);

    enum class OSR : uint8_t {
        k256 = 0x00,
        k512 = 0x02,
        k1024 = 0x04,
        k2048 = 0x06,
        k4096 = 0x08
    };

    DeviceStatus initialize() override;

    void getD1ConversionRequest(SPIBusRequest& request);
    void getD2ConversionRequest(SPIBusRequest& request);
    void getADCReadD1Request(SPIBusRequest& request);
    void getADCReadD2Request(SPIBusRequest& request);

    void parseData(BarometerData& baro_data);

    void setOSR(OSR osr);

    constexpr static uint8_t kPROMCount_ = 6;
    uint16_t prom_buffer_[kPROMCount_];

    uint8_t rx_adc_read_d1_[4];
    uint8_t rx_adc_read_d2_[4];

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

    SPIDeviceID dev_id_;

    OSR osr_ = OSR::k1024;
    PROM prom_;

    constexpr static uint8_t kCommandReset_ = 0x1E, kCommandConvD1Start_ = 0x40,
                             kCommandConvD2Start_ = 0x50,
                             kCommandReadADC_ = 0x00;

    constexpr static uint8_t kCommandReadPROMStart_ = 0xA2, kPROMStep_ = 0x02;

    constexpr static uint32_t kTimeout_ = HAL_MAX_DELAY;

    uint8_t tx_conv_d1_[1] = {kCommandConvD1Start_};
    uint8_t tx_conv_d2_[1] = {kCommandConvD2Start_};
    uint8_t tx_adc_read_[4] = {kCommandReadADC_};

    IBus::CommStatus sendCommand(uint8_t cmd);

    void getADCReadRequest_(SPIBusRequest& request);
};