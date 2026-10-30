#pragma once

#include "FreeRTOS.h"
#include "ibus.hpp"
#include "main.h"
#include "queue.h"
#include "semphr.h"

enum class CommBusType { kSPI = 0 };

enum class BusRequestType { kTxRx = 0, kTx = 1, kRx = 2 };

struct BusRequest {
    uint8_t* tx_buffer;
    uint8_t* rx_buffer;
    size_t length;
    uint32_t timeout;

    BusRequestType comm_type;
};

struct SPIBusRequest {
    BusRequest bus_request;

    GPIO_TypeDef* gpio_port;
    uint16_t cs_pin;
};

class BusHandler {
   public:
    explicit BusHandler(IBus* bus) : bus_(bus) {}

    // Functions for processing bus requests, different per handler

    virtual void execute() = 0;
    virtual IBus::CommStatus finishDMARequest() = 0;

    virtual void giveRequestSemaphore() = 0;
    bool isBusy() const { return busy_; }

   protected:
    IBus* bus_;

    QueueHandle_t request_queue_ = nullptr;
    StaticQueue_t queue_control_block_;
    // Make sure to use and implement this in child bus handlers!!
    // constexpr static uint8_t kQueueSize = {size};
    // uint8_t queue_storage_[kQueueSize * sizeof({type}BusRequest)];

    SemaphoreHandle_t request_semaphore_ = nullptr;
    StaticSemaphore_t request_semaphore_buffer_;

    // Make sure to use this in child bus handlers!!!
    // {type}BusRequest* current_request_ = nullptr;

    bool busy_ = false;
};