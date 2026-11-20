#include "barometer.hpp"

#include <cmath>
#include <cstdint>

#include "FreeRTOS.h"
#include "bus_handler.hpp"
#include "cmsis_os2.h"
#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"
#include "task.h"

Barometer::Barometer(IBus* bus, GPIO_TypeDef* cs_gpio_port, uint16_t cs_pin,
                     SPIDeviceID dev_id)
    : bus_(bus),
      cs_gpio_port_(cs_gpio_port),
      cs_pin_(cs_pin),
      dev_id_(dev_id) {}

DeviceStatus Barometer::initialize() {
    if (sendCommand(kCommandReset_) != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }

    vTaskDelay(pdMS_TO_TICKS(5));  // 2.8ms reload required

    // Read PROM
    for (uint8_t i = 0; i < kPROMCount_; i++) {
        uint8_t tx[3] = {
            static_cast<uint8_t>(kCommandReadPROMStart_ + kPROMStep_ * i)};
        uint8_t rx[3];

        HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
        bus_->transmitReceive(tx, sizeof(tx), rx, sizeof(rx), HAL_MAX_DELAY);
        HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

        prom_buffer_[i] = static_cast<uint16_t>((rx[1] << 8) | rx[2]);
    }

    // Parse PROM
    prom_.C1 = prom_buffer_[0];
    prom_.C2 = prom_buffer_[1];
    prom_.C3 = prom_buffer_[2];
    prom_.C4 = prom_buffer_[3];
    prom_.C5 = prom_buffer_[4];
    prom_.C6 = prom_buffer_[5];

    return DeviceStatus::kOk;
}

void Barometer::getD1ConversionRequest(SPIBusRequest& request) {
    request.id = dev_id_;
    BusRequest& bus_req = request.bus_request;
    bus_req.tx_buffer = tx_conv_d1_;
    bus_req.tx_length = sizeof(tx_conv_d1_);

    bus_req.comm_type = BusRequestType::kTx;

    request.gpio_port = cs_gpio_port_;
    request.cs_pin = cs_pin_;

    request.bus_request = bus_req;
}

void Barometer::getD2ConversionRequest(SPIBusRequest& request) {
    request.id = dev_id_;
    BusRequest& bus_req = request.bus_request;
    bus_req.tx_buffer = tx_conv_d2_;
    bus_req.tx_length = sizeof(tx_conv_d2_);

    bus_req.comm_type = BusRequestType::kTx;

    request.gpio_port = cs_gpio_port_;
    request.cs_pin = cs_pin_;

    request.bus_request = bus_req;
}

void Barometer::getADCReadRequest_(SPIBusRequest& request) {
    request.id = dev_id_;
    BusRequest& bus_req = request.bus_request;
    bus_req.tx_buffer = tx_adc_read_;
    bus_req.tx_length = sizeof(tx_adc_read_);
    bus_req.rx_length = sizeof(rx_adc_read_d1_);

    bus_req.comm_type = BusRequestType::kTxRx;

    request.gpio_port = cs_gpio_port_;
    request.cs_pin = cs_pin_;

    request.bus_request = bus_req;
}

void Barometer::getADCReadD1Request(SPIBusRequest& request) {
    getADCReadRequest_(request);
    request.bus_request.rx_buffer = rx_adc_read_d1_;
}

void Barometer::getADCReadD2Request(SPIBusRequest& request) {
    getADCReadRequest_(request);
    request.bus_request.rx_buffer = rx_adc_read_d2_;
}

void Barometer::setOSR(OSR osr) {
    uint8_t osr_value = static_cast<uint8_t>(osr);
    tx_conv_d1_[0] = kCommandConvD1Start_ + osr_value;
    tx_conv_d2_[0] = kCommandConvD2Start_ + osr_value;
}

void Barometer::parseData(BarometerData& baro_data) {
    uint32_t D1 = ((((static_cast<uint32_t>(rx_adc_read_d1_[1]) << 8) |
                     rx_adc_read_d1_[2])
                    << 8) |
                   rx_adc_read_d1_[3]);
    uint32_t D2 = ((((static_cast<uint32_t>(rx_adc_read_d2_[1]) << 8) |
                     rx_adc_read_d2_[2])
                    << 8) |
                   rx_adc_read_d2_[3]);

    int32_t dT = static_cast<int32_t>(D2) -
                 (static_cast<int32_t>(prom_.C5) << 8);  // D2 - C5 * 2^8

    int32_t temp =
        2000 + static_cast<int32_t>((static_cast<int64_t>(dT) * prom_.C6) >>
                                    23);  // 2000 + dT * TEMPSENS / 2^23

    int64_t OFF = prom_.C2 * pow(2, 16) +
                  (prom_.C4 * dT) / pow(2, 7);  // C2 * 2^16 + (C4 * dT) / 2^7
    int64_t SENS = prom_.C1 * pow(2, 15) +
                   (prom_.C3 * dT) / pow(2, 8);  // C1 * 2^15 + (C3 * dT) / 2^8
    int32_t P = (D1 * SENS / pow(2, 21) - OFF) /
                pow(2, 15);  // (D1 * SENS / 2^21 - OFF) / 2 ^ 15

    float pressure = static_cast<float>(P) / 100.0;
    float temperature = static_cast<float>(temp) / 100.0;

    baro_data.pressure = pressure;
    baro_data.temperature = temperature;
}

IBus::CommStatus Barometer::sendCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    IBus::CommStatus status = bus_->transmit(&cmd, 1, kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    return status;
}