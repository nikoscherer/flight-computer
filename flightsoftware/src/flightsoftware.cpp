#include "flightsoftware.hpp"
#include "main.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_def.h"
#include "stm32l4xx_hal_spi.h"

uint8_t rx_whoami[2];

void fsw_init() {
    // WHO_AM_I
    uint8_t tx_whoami[2] = { 0x0F | 0x80, 0x00 };
    
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx_whoami, rx_whoami, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    HAL_Delay(1);

    // Initialize CTRL1
    uint8_t tx_init[2] = { 0x10, 0x09 };
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx_init, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    HAL_Delay(5);
}

uint8_t rx_x_low[7];

float x_accel = 0;

void fsw_loop() {
    uint8_t tx_x_low[7] = { 0x28 | 0x80, 0, 0, 0, 0, 0, 0 };

    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx_x_low, rx_x_low, 4, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    int16_t raw_accel = static_cast<int16_t>(rx_x_low[2] << 8 | rx_x_low[1]);

    x_accel = (raw_accel * 0.061f / 1000.0f) * 9.80665f;

    
    HAL_Delay(1);
}