#pragma once

#include <stdarg.h>
#include <stdio.h>

#include "main.h"

extern UART_HandleTypeDef huart2;

// Should it really be a macro?
#define LOG_INFO(fmt, ...)                                                     \
  do {                                                                         \
    char log_buf[128];                                                         \
    int len = snprintf(log_buf, sizeof(log_buf), "[INFO] " fmt "\r\n",         \
                       ##__VA_ARGS__);                                         \
    if (len > 0) {                                                             \
      HAL_UART_Transmit(&huart2, (uint8_t *)log_buf, (uint16_t)len,            \
                        HAL_MAX_DELAY);                                        \
    }                                                                          \
  } while (0)