#include "flightsoftware.hpp"

#include "barometer.hpp"
#include "bus_handler.hpp"
#include "hardware.hpp"
#include "idevice.hpp"
#include "imu.hpp"
#include "interrupts.hpp"
#include "main.h"
#include "projdefs.h"
#include "spi_handler.hpp"
#include "stm32_spi_bus.hpp"

IMUData imu_data{};
bool imu_data_ready = false;

BarometerData baro_data{};
bool baro_data_ready = false;

DeviceStatus imu_status = DeviceStatus::kNone;
DeviceStatus baro_status = DeviceStatus::kNone;
DeviceStatus gps_status = DeviceStatus::kNone;

uint32_t gps_byte_count = 0;

void spi1TaskHandler(void* argument) {
    // IMU
    // if (imu_status = imu.initialize(); imu_status != DeviceStatus::kOk) {
    //     // Error
    //     imu_status = DeviceStatus::kError;
    // }

    // if (imu_status = imu.setConfig(imu_config);
    //     imu_status != DeviceStatus::kOk) {
    //     // Error
    //     imu_status = DeviceStatus::kError;
    // }

    // // Barometer
    // if (baro_status = baro.initialize(); baro_status != DeviceStatus::kOk) {
    //     baro_status = DeviceStatus::kError;
    // }

    // SPIBusRequest imu_request;
    // imu.getReadRequest(imu_request);
    // spi1_handler.addRequest(imu_request);

    // for (;;) {
    //     spi1_handler.execute();

    //     // Parsing happens after handler execution so we don't have a race
    //     // condition with the rx buffers.
    //     if (imu_data_ready) {
    //         imu.parseData(imu_data);
    //         imu_data_ready = false;
    //     }

    //     if (baro_data_ready) {
    //         baro.parseData(baro_data);
    //         baro_data_ready = false;
    //     }
    // }

    vTaskDelay(pdMS_TO_TICKS(HAL_MAX_DELAY));
}

void i2c1TaskHandler(void* argument) {
    // Initialize GPS
    gps_status = gps.initialize();

    for (;;) {
        if (gps.read()) {
            if (!gps.parseData()) {  // Temporary checker
                gps_status = DeviceStatus::kError;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Should use a task notifier to wait for initialization
void barometerTask(void* argument) {
    // TickType_t xFrequency = pdMS_TO_TICKS(10);  // 10 ms (temporary)

    // enum class BarometerState {
    //     kConvertD1 = 0x00,
    //     kReadD1 = 0x01,
    //     kConvertD2 = 0x02,
    //     kReadD2 = 0x03
    // };

    // BarometerState next_state = BarometerState::kConvertD2;

    // vTaskDelay(pdMS_TO_TICKS(250));  // Needs to wait for spi1 handler
    // TickType_t xLastWakeTime = xTaskGetTickCount();
    // for (;;) {
    //     SPIBusRequest baro_request;

    //     switch (next_state) {
    //         case BarometerState::kConvertD1:
    //             baro.getD1ConversionRequest(baro_request);
    //             next_state = BarometerState::kReadD1;
    //             xFrequency = pdMS_TO_TICKS(10);
    //             break;
    //         case BarometerState::kReadD1:
    //             baro.getADCReadD1Request(baro_request);
    //             next_state = BarometerState::kConvertD2;
    //             xFrequency = pdMS_TO_TICKS(10);
    //             break;
    //         case BarometerState::kConvertD2:
    //             baro.getD2ConversionRequest(baro_request);
    //             next_state = BarometerState::kReadD2;
    //             xFrequency = pdMS_TO_TICKS(10);
    //             break;
    //         case BarometerState::kReadD2:
    //             baro.getADCReadD2Request(baro_request);
    //             next_state = BarometerState::kConvertD1;
    //             xFrequency = pdMS_TO_TICKS(10);
    //             break;
    //     }

    //     spi1_handler.addRequest(baro_request);

    //     vTaskDelayUntil(&xLastWakeTime, xFrequency);
    vTaskDelay(pdMS_TO_TICKS(HAL_MAX_DELAY));
}