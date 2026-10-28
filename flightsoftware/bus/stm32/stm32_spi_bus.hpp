#pragma once

#include "ibus.hpp"
#include "main.h"
#include "stm32l4xx_hal_def.h"

class STM32SPIBus : public IBus {
   public:
    STM32SPIBus(SPI_HandleTypeDef* hspi);

    IBus::CommStatus transmitReceive(uint8_t* tx_buffer, uint8_t* rx_buffer,
                                     size_t length, uint32_t timeout) override;
    IBus::CommStatus transmit(uint8_t* tx_buffer, size_t length,
                              uint32_t timeout) override;
    IBus::CommStatus receive(uint8_t* rx_buffer, size_t length,
                             uint32_t timeout) override;

   private:
    IBus::CommStatus parseHALStatus(HAL_StatusTypeDef hal_status);

    SPI_HandleTypeDef* hspi_;
};