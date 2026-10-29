#include "flightsoftware.hpp"

#include "imu.hpp"
#include "main.h"
#include "stm32_spi_bus.hpp"

STM32SPIBus spi1_bus(&hspi1);
IMU imu(&spi1_bus, IMU_CS_GPIO_Port, IMU_CS_Pin);

void fsw_init() { imu.initialize(); }

IMUData imu_data;

bool data_ready = false;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == IMU_INT1_Pin) {
        data_ready = true;
    }
}

uint32_t count = 0;
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
    if (hspi->Instance == SPI1) {
        HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

        imu.parseData(imu_data);
        count++;
    }
}

void fsw_loop() {
    if (data_ready) {
        imu.read();
        data_ready = false;
    }
}