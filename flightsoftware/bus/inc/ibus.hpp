#pragma once

#include <stddef.h>
#include <stdint.h>

class IBus {
   public:
    enum class CommStatus { kOk = 0, kTimeout = 1, kBusy = 2, kError = 3 };

    virtual CommStatus transmitReceiveDMA(const uint8_t* tx_buffer,
                                          size_t tx_length, uint8_t* rx_buffer,
                                          size_t rx_length) {
        return IBus::CommStatus::kError;
    }

    virtual CommStatus transmitDMA(const uint8_t* tx_buffer, size_t tx_length) {
        return IBus::CommStatus::kError;
    }
    virtual CommStatus receiveDMA(uint8_t* rx_buffer, size_t rx_length) {
        return IBus::CommStatus::kError;
    }

    virtual CommStatus transmitReceive(const uint8_t* tx_buffer,
                                       size_t tx_length, uint8_t* rx_buffer,
                                       size_t rx_length, uint32_t timeout) {
        return IBus::CommStatus::kError;
    }
    virtual CommStatus transmit(uint8_t device_address,
                                const uint8_t* tx_buffer, size_t tx_length,
                                uint32_t timeout) {
        return IBus::CommStatus::kError;
    }
    virtual CommStatus transmit(const uint8_t* tx_buffer, size_t tx_length,
                                uint32_t timeout) {
        return IBus::CommStatus::kError;
    }
    virtual CommStatus receive(uint8_t device_address, uint8_t reg,
                               uint8_t* rx_buffer, size_t rx_length,
                               uint32_t timeout) {
        return IBus::CommStatus::kError;
    }
    virtual CommStatus receive(uint8_t* rx_buffer, size_t rx_length,
                               uint32_t timeout) {
        return IBus::CommStatus::kError;
    }

    virtual CommStatus getDMAStatus() = 0;
};