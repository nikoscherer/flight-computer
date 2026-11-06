#include "barometer.hpp"

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"
#include "task.h"

Barometer::Barometer(IBus* bus, GPIO_TypeDef* cs_gpio_port, uint16_t cs_pin)
    : bus_(bus), cs_gpio_port_(cs_gpio_port), cs_pin_(cs_pin) {}

DeviceStatus Barometer::initialize() {
    uint8_t tx_reset = kCommandReset_;
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    bus_->transmit(&tx_reset, 1, HAL_MAX_DELAY);  // Should be set delay
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);
    vTaskDelay(pdMS_TO_TICKS(5));  // 2.8ms reload required

    for (uint8_t i = 0; i < kPROMCount_; i++) {
        uint8_t tx[3] = {
            static_cast<uint8_t>(kCommandReadPROMStart_ + kPROMStep_ * i)};
        uint8_t rx[3];

        HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
        bus_->transmitReceive(tx, rx, sizeof(tx), HAL_MAX_DELAY);
        HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

        prom_buffer_[i] = static_cast<uint16_t>((rx[1] << 8) | rx[2]);
    }

    return DeviceStatus::kOk;
}