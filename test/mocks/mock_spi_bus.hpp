#pragma once

#include <unordered_map>

#include "ibus.hpp"

class MockSPIBus : public IBus {
   public:
    MockSPIBus(std::unordered_map<uint8_t, uint8_t> register_map);

    IBus::CommStatus transmitReceive(uint8_t* tx_buffer, uint8_t* rx_buffer,
                                     size_t length, uint32_t timeout) override;
    IBus::CommStatus transmit(uint8_t* tx_buffer, size_t length,
                              uint32_t timeout) override;
    IBus::CommStatus receive(uint8_t* rx_buffer, size_t length,
                             uint32_t timeout) override;

   private:
    std::unordered_map<uint8_t, uint8_t> register_map_;
};