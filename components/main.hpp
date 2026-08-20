#pragma once

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;

int app_main(void);

#ifdef __cplusplus
}
#endif

void flightTask(void *pvParameters);