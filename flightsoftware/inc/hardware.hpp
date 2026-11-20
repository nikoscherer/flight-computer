#pragma once

#include "barometer.hpp"
#include "gps.hpp"
#include "imu.hpp"
#include "main.h"
#include "spi_handler.hpp"
#include "stm32_i2c_bus.hpp"
#include "stm32_spi_bus.hpp"

extern SPI_HandleTypeDef hspi1;
extern STM32SPIBus spi1_bus;
extern SPIHandler spi1_handler;

extern I2C_HandleTypeDef hi2c1;
extern STM32I2CBus i2c1_bus;

extern IMU::Config imu_config;
extern IMU imu;

extern Barometer baro;

extern GPS gps;