#include "bus_handler.hpp"

#include "ibus.hpp"

BusHandler::BusHandler(IBus* bus) : bus_(bus) {}

/*
    This currently only works with blocking commands.
*/
bool BusHandler::processRequest(BusRequest& request) {
    // This should be through an interface, we don't want to use HAL functions
    // in this class.
    if (request.bus_type == CommBusType::kSPI) {
        HAL_GPIO_WritePin(request.cs_gpio_port, request.cs_pin, GPIO_PIN_RESET);
    }

    // Handle transmission
    IBus::CommStatus status;
    switch (request.comm_type) {
        case BusRequestType::kTxRx:
            status = bus_->transmitReceive(request.tx_buffer, request.rx_buffer,
                                           request.length, request.timeout);
            break;
        case BusRequestType::kTx:
            bus_->transmit(request.tx_buffer, request.length, request.timeout);
            break;
        case BusRequestType::kRx:
            bus_->receive(request.rx_buffer, request.length, request.timeout);
            break;
    }

    if (request.bus_type == CommBusType::kSPI) {
        HAL_GPIO_WritePin(request.cs_gpio_port, request.cs_pin, GPIO_PIN_SET);
    }

    return status == IBus::CommStatus::kOk;
}