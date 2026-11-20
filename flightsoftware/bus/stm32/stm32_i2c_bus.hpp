#pragma once

#include "ibus.hpp"
#include "main.h"

class STM32I2CBus : public IBus {
   public:
    STM32I2CBus(I2C_HandleTypeDef* hi2c);

    IBus::CommStatus transmitReceiveDMA(const uint8_t* tx_buffer,
                                        size_t tx_length, uint8_t* rx_buffer,
                                        size_t rx_length) override;
    IBus::CommStatus transmitDMA(const uint8_t* tx_buffer,
                                 size_t tx_length) override;
    IBus::CommStatus receiveDMA(uint8_t* rx_buffer, size_t rx_length) override;

    IBus::CommStatus transmitReceive(const uint8_t* tx_buffer, size_t tx_length,
                                     uint8_t* rx_buffer, size_t rx_length,
                                     uint32_t timeout) override;
    IBus::CommStatus transmit(uint8_t device_address, const uint8_t* tx_buffer,
                              size_t tx_length, uint32_t timeout) override;
    IBus::CommStatus receive(uint8_t device_address, uint8_t reg,
                             uint8_t* rx_buffer, size_t rx_length,
                             uint32_t timeout) override;

    IBus::CommStatus getDMAStatus() override;

   private:
    IBus::CommStatus parseHALStatus(HAL_StatusTypeDef hal_status);

    I2C_HandleTypeDef* hi2c_;
};