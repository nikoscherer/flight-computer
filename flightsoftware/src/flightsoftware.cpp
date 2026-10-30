#include "flightsoftware.hpp"

#include "imu.hpp"
#include "main.h"
#include "spi_handler.hpp"
#include "stm32_spi_bus.hpp"

STM32SPIBus spi1_bus(&hspi1);
SPIHandler spi1_handler(&spi1_bus);

IMU imu(&spi1_bus, IMU_CS_GPIO_Port, IMU_CS_Pin);
IMUData imu_data;

uint32_t data_count = 0;

void fsw_init() {
    imu.initialize();
    if (HAL_GPIO_ReadPin(IMU_INT1_GPIO_Port, IMU_INT1_Pin)) {
        SPIBusRequest imu_request;
        imu.getReadRequest(imu_request);
        spi1_handler.addRequest(imu_request);
    }
}

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

void spi1TaskHandler(void* argument) {
    for (;;) {
        spi1_handler.execute();

        // handle data here?
        imu.parseData(imu_data);
        data_count++;
    }
}

void fsw_loop() {
    // if (d!spi1_handler.isBusy()) {
    //     imu.getReadRequest(imu_request);
    //     spi1_handler.processBusRequest(imu_request);
    //     data_ready = false;
    // }
}