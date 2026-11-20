#pragma once

#include <atomic>

#include "barometer.hpp"
#include "imu.hpp"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void spi1TaskHandler(void* argument);
void i2c1TaskHandler(void* argument);
void barometerTask(void* argument);

extern IMUData imu_data;
extern bool imu_data_ready;

extern BarometerData baro_data;
extern bool baro_data_ready;

#ifdef __cplusplus
}
#endif