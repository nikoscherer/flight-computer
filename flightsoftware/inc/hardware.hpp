#pragma once

#include "barometer.hpp"
#include "imu.hpp"
#include "main.h"
#include "spi_handler.hpp"
#include "stm32_spi_bus.hpp"

extern SPI_HandleTypeDef hspi1;

extern STM32SPIBus spi1_bus;
extern SPIHandler spi1_handler;

extern IMU::Config imu_config;
extern IMU imu;

extern Barometer baro;