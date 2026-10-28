#include "flightsoftware.hpp"

#include "bus_handler.hpp"
#include "ibus.hpp"
#include "imu.hpp"
#include "main.h"
#include "stm32_spi_bus.hpp"

STM32SPIBus spi1 = STM32SPIBus(&hspi1);
BusHandler spi1_handler = BusHandler(&spi1);

IMU imu = IMU(CommBusType::kSPI);

uint8_t rx_whoami[2];
void fsw_init() {
    // Temporary initialization for IMU
    uint8_t tx_whoami[2] = {0x0F | 0x80, 0x00};

    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx_whoami, rx_whoami, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    HAL_Delay(1);

    // Initialize CTRL1
    uint8_t tx_init[2] = {0x10, 0x09};
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx_init, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    HAL_Delay(5);
}

IMUData imu_data;
int fail_count = 0;

void fsw_loop() {
    BusRequest imu_read_request;
    imu.requestRead(imu_read_request);

    imu.getAccel(imu_data);

    HAL_Delay(1);
}