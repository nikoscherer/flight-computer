#pragma once

#include "ibus.hpp"
#include "main.h"

struct SPIBusRequest {
    BusRequest bus_request;

    GPIO_TypeDef* gpio_port;
    uint16_t cs_pin;
};

class SPIHandler {
   public:
    SPIHandler(IBus* bus);

    // Temporary bus request handling, once FreeRTOS is implemented we should be
    // using a queue and semaphore.
    void processBusRequest(SPIBusRequest& bus_request);

    void finishDMARequest();

    bool isBusy() const { return busy_; }

   private:
    IBus* bus_;

    SPIBusRequest* current_request;

    bool busy_ = false;
};