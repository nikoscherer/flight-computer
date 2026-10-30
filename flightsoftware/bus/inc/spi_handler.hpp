#pragma once

#include "FreeRTOS.h"
#include "bus_handler.hpp"
#include "ibus.hpp"
#include "main.h"
#include "queue.h"
#include "semphr.h"

class SPIHandler : public BusHandler {
   public:
    explicit SPIHandler(IBus* bus);

    void execute() override;
    IBus::CommStatus finishDMARequest() override;

    bool addRequestFromISR(const SPIBusRequest& bus_request);
    bool addRequest(const SPIBusRequest& bus_request);

    void processBusRequest(SPIBusRequest& bus_request);

    void giveRequestSemaphore() override;

   private:
    constexpr static uint8_t kQueueSize = 10;
    uint8_t queue_storage_[kQueueSize * sizeof(SPIBusRequest)];

    SPIBusRequest* current_request_ = nullptr;

    bool busy_ = false;
};