#pragma once

#include "ibus.hpp"
#include "main.h"

class STM32SPIBus : public IBus {
   public:
    STM32SPIBus(SPI_HandleTypeDef* hspi);

    IBus::CommStatus transmitReceiveDMA(const uint8_t* tx_buffer,
                                        uint8_t* rx_buffer,
                                        size_t length) override;
    IBus::CommStatus transmitDMA(const uint8_t* tx_buffer,
                                 size_t length) override;
    IBus::CommStatus receiveDMA(uint8_t* rx_buffer, size_t length) override;

    IBus::CommStatus transmitReceive(const uint8_t* tx_buffer,
                                     uint8_t* rx_buffer, size_t length,
                                     uint32_t timeout) override;
    IBus::CommStatus transmit(const uint8_t* tx_buffer, size_t length,
                              uint32_t timeout) override;
    IBus::CommStatus receive(uint8_t* rx_buffer, size_t length,
                             uint32_t timeout) override;

    IBus::CommStatus getDMAStatus() override;

   private:
    IBus::CommStatus parseHALStatus(HAL_StatusTypeDef hal_status);

    SPI_HandleTypeDef* hspi_;
};