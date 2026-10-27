#pragma once

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif
extern SPI_HandleTypeDef hspi1;

void fsw_init();
void fsw_loop();

#ifdef __cplusplus
}
#endif