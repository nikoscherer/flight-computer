#pragma once

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif
extern SPI_HandleTypeDef hspi1;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi);

void fsw_init();
void fsw_loop();

#ifdef __cplusplus
}
#endif