#include "flightsoftware.hpp"

#include "barometer.hpp"
#include "idevice.hpp"
#include "imu.hpp"
#include "main.h"
#include "projdefs.h"
#include "spi_handler.hpp"
#include "stm32_spi_bus.hpp"

STM32SPIBus spi1_bus(&hspi1);
SPIHandler spi1_handler(&spi1_bus);

IMU imu(&spi1_bus, IMU_CS_GPIO_Port, IMU_CS_Pin);
IMUData imu_data;

uint32_t data_count = 0;
void fsw_init() {}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == IMU_INT1_Pin) {  // IMU ready
        SPIBusRequest imu_request;
        imu.getReadRequest(imu_request);
        spi1_handler.addRequestFromISR(imu_request);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
    if (hspi->Instance == SPI1) {
        spi1_handler.giveRequestSemaphore();
    }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi) {}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi) {}

DeviceStatus imu_status = DeviceStatus::kNone;
uint32_t invalid_count = 0;
uint32_t valid_count = 0;
void spi1TaskHandler(void* argument) {
    // IMU
    if (imu_status = imu.initialize(); imu_status != DeviceStatus::kOk) {
        // Error
    }

    // if (HAL_GPIO_ReadPin(IMU_INT1_GPIO_Port, IMU_INT1_Pin)) {
    //     SPIBusRequest imu_request;
    //     imu.getReadRequest(imu_request);
    //     spi1_handler.addRequest(imu_request);
    // }

    for (;;) {
        // spi1_handler.execute();

        imu.read();

        // // handle data here?
        imu.parseData(imu_data);
        data_count++;
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void fsw_loop() {
    // if (!spi1_handler.isBusy()) {
    //     imu.getReadRequest(imu_request);
    //     spi1_handler.processBusRequest(imu_request);
    //     data_ready = false;
    // }
}