#include "flightsoftware.hpp"

#include "barometer.hpp"
#include "idevice.hpp"
#include "imu.hpp"
#include "main.h"
#include "spi_handler.hpp"
#include "stm32_spi_bus.hpp"

STM32SPIBus spi1_bus(&hspi1);
SPIHandler spi1_handler(&spi1_bus);

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
IMU imu(&spi1_bus, IMU_CS_GPIO_Port, IMU_CS_Pin);
IMUData imu_data;

uint32_t data_count = 0;
void fsw_init() {}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == IMU_INT1_Pin) {  // IMU ready
        SPIBusRequest imu_request;
        imu.getReadRequest(imu_request);
        spi1_handler.addRequestFromISR(imu_request);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
    if (hspi->Instance == SPI1) {
        spi1_handler.giveRequestSemaphore();
    }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi) {}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi) {}

DeviceStatus imu_status = DeviceStatus::kNone;
void spi1TaskHandler(void* argument) {
    // IMU
    if (imu_status = imu.initialize(); imu_status != DeviceStatus::kOk) {
        // Error
        imu_status = DeviceStatus::kError;
    }

    if (imu_status = imu.setConfig(imu_config);
        imu_status != DeviceStatus::kOk) {
        // Error
        imu_status = DeviceStatus::kError;
    }

    SPIBusRequest imu_request;
    imu.getReadRequest(imu_request);
    spi1_handler.addRequest(imu_request);

    for (;;) {
        spi1_handler.execute();

        imu.parseData(imu_data);
        data_count++;
    }
}

void fsw_loop() {}