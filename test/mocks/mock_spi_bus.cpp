#include "mock_spi_bus.hpp"

#include <unordered_map>

MockSPIBus::MockSPIBus(std::unordered_map<uint8_t, uint8_t> register_map)
    : register_map_(register_map) {}

IBus::CommStatus MockSPIBus::transmitReceive(uint8_t* tx_buffer,
                                             uint8_t* rx_buffer, size_t length,
                                             uint32_t timeout) {
    for (int i = 0; i < length; i++) {
        rx_buffer[i] = register_map_[tx_buffer[i]];
    }
    return IBus::CommStatus::kOk;
}

IBus::CommStatus MockSPIBus::transmit(uint8_t* tx_buffer, size_t length,
                                      uint32_t timeout) {
    return IBus::CommStatus::kError;
}

IBus::CommStatus MockSPIBus::receive(uint8_t* rx_buffer, size_t length,
                                     uint32_t timeout) {
    return IBus::CommStatus::kError;
}