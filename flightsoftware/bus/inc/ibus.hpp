#pragma once

#include <cstddef>
#include <cstdint>

enum class CommBusType { kSPI = 0 };

enum class BusRequestType { kTxRx = 0, kTx = 1, kRx = 2 };

struct BusRequest {
    uint8_t* tx_buffer;
    uint8_t* rx_buffer;
    size_t length;
    uint32_t timeout;

    int* cs_gpio_port;
    uint16_t cs_pin;

    BusRequestType comm_type;
    CommBusType bus_type;
};

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
};