#include "flight_core.hpp"

#include "adxl343.hpp"
#include "debug.hpp"
#include "isensor.hpp"
#include "stm32_i2c_bus.hpp"

STM32I2CBus i2c1_bus(&hi2c1, 0x77 << 1);

ADXL343 adxl343(&i2c1_bus);

int fsw_main() {
  // Initialize the flight software
  ISensor::Status adxl343_status = adxl343.init();
  if (adxl343_status != ISensor::Status::kOk) {
    LOG_ERROR("Failed to initialize ADXL343!");
  }

  return 0;
}

int fsw_loop() {
  for (;;) {
    // Main flight software loop
  }
}