#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "main.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart2;

int fsw_main();
int fsw_loop();

#ifdef __cplusplus
}
#endif