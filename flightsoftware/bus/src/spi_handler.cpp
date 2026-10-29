#include "spi_handler.hpp"

#include "ibus.hpp"
#include "main.h"

SPIHandler::SPIHandler(IBus* bus) : bus_(bus) {}

void SPIHandler::processBusRequest(SPIBusRequest& request) {
    HAL_GPIO_WritePin(request.gpio_port, request.cs_pin, GPIO_PIN_RESET);

    BusRequest& bus_request = request.bus_request;
    switch (request.bus_request.comm_type) {
        case BusRequestType::kTxRx:
            bus_->transmitReceiveDMA(bus_request.tx_buffer,
                                     bus_request.rx_buffer, bus_request.length);
            break;
        case BusRequestType::kTx:
            bus_->transmitDMA(bus_request.tx_buffer, bus_request.length);
            break;
        case BusRequestType::kRx:
            bus_->receiveDMA(bus_request.rx_buffer, bus_request.length);
            break;
    }

    current_request = &request;
    busy_ = true;
}

void SPIHandler::finishDMARequest() {
    HAL_GPIO_WritePin(current_request->gpio_port, current_request->cs_pin,
                      GPIO_PIN_SET);

    busy_ = false;
}