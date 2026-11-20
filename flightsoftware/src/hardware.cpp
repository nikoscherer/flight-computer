#include "hardware.hpp"

#include "flightsoftware.hpp"
#include "gps.hpp"
#include "stm32_i2c_bus.hpp"

STM32SPIBus spi1_bus(&hspi1);
SPIHandler spi1_handler(&spi1_bus);

STM32I2CBus i2c1_bus(&hi2c1);

IMU::Config imu_config{
    .accel_config{.odr = IMU::AccelODR::k1_92kHz,
                  .fs_config = IMU::getFullScaleConfig(IMU::AccelFS::k4G)},
    .accel_hi_g_config{
        .odr = IMU::HiGAccelODR::kPowerDown,
        .fs_config = IMU::getFullScaleConfig(IMU::HiGAccelFS::k32G)},
    .gyro_config{
        .odr = IMU::GyroODR::k1_92kHz,
        .fs_config = IMU::getFullScaleConfig(IMU::GyroFS::k500DPS),
    }};
IMU imu(&spi1_bus, IMU_CS_GPIO_Port, IMU_CS_Pin, SPIDeviceID::kIMU);

Barometer baro(&spi1_bus, BARO_CS_GPIO_Port, BARO_CS_Pin,
               SPIDeviceID::kBarometer);

GPS gps(&i2c1_bus);