#include "flight_core.hpp"

#include "stm32_i2c_bus.hpp"

STM32I2CBus i2c1_bus(&hi2c1, 0x77 << 1);

int fsw_main() {
  // Initialize the flight software

  return 0;
}

int fsw_loop() {
  for (;;) {
  }
}