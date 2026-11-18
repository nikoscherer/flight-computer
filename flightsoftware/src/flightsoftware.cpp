#include "flightsoftware.hpp"

#include "barometer.hpp"
#include "bus_handler.hpp"
#include "hardware.hpp"
#include "idevice.hpp"
#include "imu.hpp"
#include "main.h"
#include "spi_handler.hpp"
#include "stm32_spi_bus.hpp"


IMUData imu_data{};
bool imu_data_ready = false;

BarometerData baro_data{};
bool baro_data_ready = false;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == IMU_INT1_Pin) {  // IMU ready
        SPIBusRequest imu_request;
        imu.getReadRequest(imu_request);
        spi1_handler.addRequestFromISR(imu_request);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
    if (hspi->Instance == SPI1) {
        SPIBusRequest request = spi1_handler.getCurrentRequest();
        switch (request.id) {
            case kIMU:
                imu_data_ready = true;
                break;
            case kBarometer:
                baro_data_ready = true;
                break;
        }

        spi1_handler.giveRequestSemaphore();
    }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi) {
    if (hspi->Instance == SPI1) {
        spi1_handler.giveRequestSemaphore();
    }
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi) {
    if (hspi->Instance == SPI1) {
        spi1_handler.giveRequestSemaphore();
    }
}

DeviceStatus imu_status = DeviceStatus::kNone;
DeviceStatus baro_status = DeviceStatus::kNone;

void spi1TaskHandler(void* argument) {
    // IMU
    if (imu_status = imu.initialize(); imu_status != DeviceStatus::kOk) {
        // Error
        imu_status = DeviceStatus::kError;
    }

    if (imu_status = imu.setConfig(imu_config);
        imu_status != DeviceStatus::kOk) {
        // Error
        imu_status = DeviceStatus::kError;
    }

    // Barometer
    if (baro_status = baro.initialize(); baro_status != DeviceStatus::kOk) {
        baro_status = DeviceStatus::kError;
    }

    SPIBusRequest imu_request;
    imu.getReadRequest(imu_request);
    spi1_handler.addRequest(imu_request);

    for (;;) {
        spi1_handler.execute();

        if (imu_data_ready) {
            imu.parseData(imu_data);
            imu_data_ready = false;
        }

        if (baro_data_ready) {
            baro.parseData(baro_data);
            baro_data_ready = false;
        }
    }
}

// Should use a task notifier to wait for initialization
void barometerTask(void* argument) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    TickType_t xFrequency = pdMS_TO_TICKS(10);  // 10 ms (temporary)

    enum class BarometerState {
        kConvertD1 = 0x00,
        kReadD1 = 0x01,
        kConvertD2 = 0x02,
        kReadD2 = 0x03
    };

    BarometerState next_state = BarometerState::kConvertD2;

    vTaskDelay(pdMS_TO_TICKS(250));  // Needs to wait for spi1 handler
    for (;;) {
        SPIBusRequest baro_request;

        switch (next_state) {
            case BarometerState::kConvertD1:
                baro.getD1ConversionRequest(baro_request);
                next_state = BarometerState::kReadD1;
                break;
            case BarometerState::kReadD1:
                baro.getADCReadD1Request(baro_request);
                next_state = BarometerState::kConvertD2;
                break;
            case BarometerState::kConvertD2:
                baro.getD2ConversionRequest(baro_request);
                next_state = BarometerState::kReadD2;
                break;
            case BarometerState::kReadD2:
                baro.getADCReadD2Request(baro_request);
                next_state = BarometerState::kConvertD1;
                break;
        }

        spi1_handler.addRequest(baro_request);

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}