#include "spi_handler.hpp"

#include "bus_handler.hpp"
#include "ibus.hpp"
#include "main.h"

SPIHandler::SPIHandler(IBus* bus) : BusHandler(bus) {
    request_queue_ = xQueueCreateStatic(kQueueSize, sizeof(SPIBusRequest),
                                        queue_storage_, &queue_control_block_);
    request_semaphore_ =
        xSemaphoreCreateBinaryStatic(&request_semaphore_buffer_);
}

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

    current_request_ = &request;
    busy_ = true;
}

bool SPIHandler::addRequestFromISR(const SPIBusRequest& bus_request) {
    return xQueueSendFromISR(request_queue_, &bus_request, 0) == pdPASS;
}

bool SPIHandler::addRequest(const SPIBusRequest& bus_request) {
    return xQueueSend(request_queue_, &bus_request, 0) == pdPASS;
}

void SPIHandler::giveRequestSemaphore() {
    BaseType_t xHigherPriorityTaskWoken;
    xSemaphoreGiveFromISR(request_semaphore_, &xHigherPriorityTaskWoken);
}

void SPIHandler::execute() {
    SPIBusRequest request;
    if (xQueueReceive(request_queue_, &request, portMAX_DELAY)) {
        processBusRequest(request);

        if (xSemaphoreTake(request_semaphore_, portMAX_DELAY)) {
            // We can reset fault status in here (or set a variable to return)
        } else {
            // Handle fault status code
        }

        finishDMARequest();  // This MUST run, we have to set the gpio pin high
                             // after the transfer.
    }
}

IBus::CommStatus SPIHandler::finishDMARequest() {
    HAL_GPIO_WritePin(current_request_->gpio_port, current_request_->cs_pin,
                      GPIO_PIN_SET);

    busy_ = false;

    return bus_->getDMAStatus();
}