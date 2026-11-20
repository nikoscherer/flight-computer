#include "mock_spi_bus.hpp"

#include <unordered_map>

MockSPIBus::MockSPIBus(std::unordered_map<uint8_t, uint8_t> register_map)
    : register_map_(register_map) {}

IBus::CommStatus MockSPIBus::transmitReceiveDMA(const uint8_t* tx_buffer,
                                                size_t tx_length,
                                                uint8_t* rx_buffer,
                                                size_t rx_length) {
    return IBus::CommStatus::kOk;
}

IBus::CommStatus MockSPIBus::transmitDMA(const uint8_t* tx_buffer,
                                         size_t tx_length) {
    return IBus::CommStatus::kOk;
}

IBus::CommStatus MockSPIBus::receiveDMA(uint8_t* rx_buffer, size_t rx_length) {
    return IBus::CommStatus::kOk;
}

IBus::CommStatus MockSPIBus::transmitReceive(const uint8_t* tx_buffer,
                                             size_t tx_length,
                                             uint8_t* rx_buffer,
                                             size_t rx_length,
                                             uint32_t timeout) {
    size_t length = rx_length < tx_length ? rx_length : tx_length;
    for (size_t i = 0; i < length; i++) {
        rx_buffer[i] = register_map_[tx_buffer[i]];
    }
    return IBus::CommStatus::kOk;
}

IBus::CommStatus MockSPIBus::transmit(const uint8_t* tx_buffer,
                                      size_t tx_length, uint32_t timeout) {
    return IBus::CommStatus::kError;
}

IBus::CommStatus MockSPIBus::receive(uint8_t* rx_buffer, size_t rx_length,
                                     uint32_t timeout) {
    return IBus::CommStatus::kError;
}

IBus::CommStatus MockSPIBus::getDMAStatus() { return IBus::CommStatus::kOk; }