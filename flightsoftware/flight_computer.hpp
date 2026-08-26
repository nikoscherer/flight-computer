#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi2;

void flightTask(void *pvParameters);

int fsw_main();

#ifdef __cplusplus
}
#endif