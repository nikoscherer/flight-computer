#pragma once

#include <cstddef>
#include <cstdint>

class IBus {
   public:
    enum class CommStatus { kOk = 0, kTimeout = 1, kBusy = 2, kError = 3 };

    virtual CommStatus transmitReceiveDMA(uint8_t* tx_buffer,
                                          uint8_t* rx_buffer,
                                          size_t length) = 0;

    virtual CommStatus transmitDMA(uint8_t* tx_buffer, size_t length) = 0;
    virtual CommStatus receiveDMA(uint8_t* rx_buffer, size_t length) = 0;

    virtual CommStatus transmitReceive(uint8_t* tx_buffer, uint8_t* rx_buffer,
                                       size_t length, uint32_t timeout) = 0;
    virtual CommStatus transmit(uint8_t* tx_buffer, size_t length,
                                uint32_t timeout) = 0;
    virtual CommStatus receive(uint8_t* rx_buffer, size_t length,
                               uint32_t timeout) = 0;

    virtual CommStatus getDMAStatus() = 0;
};