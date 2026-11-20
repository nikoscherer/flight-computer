#pragma once

#include <unordered_map>

#include "ibus.hpp"

class MockSPIBus : public IBus {
   public:
    MockSPIBus(std::unordered_map<uint8_t, uint8_t> register_map);

    IBus::CommStatus transmitReceiveDMA(const uint8_t* tx_buffer,
                                        size_t tx_length, uint8_t* rx_buffer,
                                        size_t rx_length) override;
    IBus::CommStatus transmitDMA(const uint8_t* tx_buffer,
                                 size_t tx_length) override;
    IBus::CommStatus receiveDMA(uint8_t* rx_buffer, size_t rx_length) override;
    IBus::CommStatus transmitReceive(const uint8_t* tx_buffer, size_t tx_length,
                                     uint8_t* rx_buffer, size_t rx_length,
                                     uint32_t timeout) override;
    IBus::CommStatus transmit(const uint8_t* tx_buffer, size_t tx_length,
                              uint32_t timeout) override;
    IBus::CommStatus receive(uint8_t* rx_buffer, size_t rx_length,
                             uint32_t timeout) override;
    IBus::CommStatus getDMAStatus() override;

   private:
    std::unordered_map<uint8_t, uint8_t> register_map_;
};