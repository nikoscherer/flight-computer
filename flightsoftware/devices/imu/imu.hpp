#pragma once

#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"
#include "spi_handler.hpp"

struct IMUData {
    float x_accel;
    float y_accel;
    float z_accel;

    float pitch_vel;
    float roll_vel;
    float yaw_vel;
};

class IMU : public IDevice {
   public:
    IMU(IBus* bus, GPIO_TypeDef* cs_gpio_port, uint16_t cs_pin);

    // IMU Config
    // Accelerometer
    enum class AccelODR : uint8_t {
        kPowerDown = 0x00,
        k1_92kHz = 0x0A,
        k7_68kHz = 0x0C
    };
    enum class AccelFS : uint8_t {
        k2G = 0x00,
        k4G = 0x01,
        k8G = 0x02,
        k16G = 0x03
    };
    struct AccelFullScaleConfig {
        AccelFS fs;
        float sens_mg_lsb;
    };
    struct AccelConfig {
        AccelODR odr;
        AccelFullScaleConfig fs_config;
    };
    constexpr static AccelFullScaleConfig getFullScaleConfig(AccelFS accel_fs) {
        AccelFullScaleConfig cfg;
        cfg.fs = accel_fs;
        switch (accel_fs) {
            case AccelFS::k2G:
                cfg.sens_mg_lsb = 0.061f;
                break;
            case AccelFS::k4G:
                cfg.sens_mg_lsb = 0.122f;
                break;
            case AccelFS::k8G:
                cfg.sens_mg_lsb = 0.244f;
                break;
            case AccelFS::k16G:
                cfg.sens_mg_lsb = 0.488f;
                break;
        }

        return cfg;
    }

    // High-G Accelerometer
    enum class HiGAccelODR : uint8_t {
        kPowerDown = 0x00,
        k1_92kHz = 0x05,
        k7_68kHz = 0x07
    };
    enum class HiGAccelFS : uint8_t {
        k32G = 0x00,
        k64G = 0x01,
        k128G = 0x02,
        k256G = 0x03,
        k320G = 0x04
    };
    struct HiGAccelFullScaleConfig {
        HiGAccelFS fs;
        float sens_mg_lsb;
    };
    struct HiGAccelConfig {
        HiGAccelODR odr;
        HiGAccelFullScaleConfig fs_config;
    };
    constexpr static HiGAccelFullScaleConfig getFullScaleConfig(
        HiGAccelFS accel_hg_fs) {
        HiGAccelFullScaleConfig cfg;
        cfg.fs = accel_hg_fs;
        switch (accel_hg_fs) {
            case HiGAccelFS::k32G:
                cfg.sens_mg_lsb = 0.976f;
                break;
            case HiGAccelFS::k64G:
                cfg.sens_mg_lsb = 1.952f;
                break;
            case HiGAccelFS::k128G:
                cfg.sens_mg_lsb = 3.904f;
                break;
            case HiGAccelFS::k256G:
                cfg.sens_mg_lsb = 7.808f;
                break;
            case HiGAccelFS::k320G:
                cfg.sens_mg_lsb = 10.417f;
                break;
        }

        return cfg;
    }

    // Gyroscope
    enum class GyroODR : uint8_t {
        kPowerDown = 0x00,
        k1_92kHz = 0x0A,
        k7_68kHz = 0x0C
    };
    enum class GyroFS : uint8_t {
        k250DPS = 0x00,
        k500DPS = 0x01,
        k1000DPS = 0x03,
        k2000DPS = 0x04,
        k4000DPS = 0x05
    };
    struct GyroFullScaleConfig {
        GyroFS fs;
        float sens_mg_lsb;
    };
    struct GyroConfig {
        GyroODR odr;
        GyroFullScaleConfig fs_config;
    };
    constexpr static GyroFullScaleConfig getFullScaleConfig(GyroFS gyro_fs) {
        GyroFullScaleConfig cfg;
        cfg.fs = gyro_fs;
        switch (gyro_fs) {
            case GyroFS::k250DPS:
                cfg.sens_mg_lsb = 8.75f;
                break;
            case GyroFS::k500DPS:
                cfg.sens_mg_lsb = 17.50f;
                break;
            case GyroFS::k1000DPS:
                cfg.sens_mg_lsb = 35.0f;
                break;
            case GyroFS::k2000DPS:
                cfg.sens_mg_lsb = 70.0f;
                break;
            case GyroFS::k4000DPS:
                cfg.sens_mg_lsb = 140.0f;
                break;
        }

        return cfg;
    }
    struct Config {
        AccelConfig accel_config;
        HiGAccelConfig accel_hi_g_config;
        GyroConfig gyro_config;
    };

    DeviceStatus initialize() override;
    // DeviceStatus reset() override;

    DeviceStatus setConfig(const Config& config);

    DeviceStatus read();
    void getReadRequest(SPIBusRequest& bus_request);

    void parseData(IMUData& imu_data);

    DeviceStatus checkWhoAmI();

    IBus::CommStatus setAccelFS(AccelFS accel_fs);
    IBus::CommStatus setAccelODR(AccelODR accel_odr);

    IBus::CommStatus setHiGAccelFS(HiGAccelFS hg_accel_fs);
    IBus::CommStatus setHiGAccelODR(HiGAccelODR hg_accel_odr);

    IBus::CommStatus setGyroFS(GyroFS gyro_fs);
    IBus::CommStatus setGyroODR(GyroODR gyro_odr);

   private:
    IBus* bus_;

    GPIO_TypeDef* cs_gpio_port_;
    uint16_t cs_pin_;

    constexpr static uint8_t kDeviceAddress_ = 0x73;

    constexpr static uint8_t kReadRegisterPrefix_ = 0x80;

    // Registers
    constexpr static uint8_t kRegisterIF_CFG_ = 0x03, kRegisterWHO_AM_I_ = 0x0F,
                             kRegisterCTRL_1_ = 0x10, kRegisterCTRL_2_ = 0x11,
                             kRegisterCTRL_3_ = 0x12, kRegisterCTRL_6_ = 0x15,
                             kRegisterCTRL_8_ = 0x17,
                             kRegisterCTRL_1_XL_HG_ = 0x4E;

    constexpr static uint32_t kTimeout_ = HAL_MAX_DELAY;

    constexpr static uint8_t kRegisterINT1_CTRL_ = 0x0D,
                             // Read registers
        kRegisterOUTX_L_G_ = 0x22, kRegisterOUTX_L_A_ = 0x28,
                             kRegisterLengthOUT_L_A_ = 6,
                             kRegisterLengthOUT_L_G_ = 6;

    constexpr static uint8_t kTxRxReadLength_ =
        1 + kRegisterLengthOUT_L_G_ + kRegisterLengthOUT_L_A_;

    uint8_t tx_read_buffer_[kTxRxReadLength_] = {kRegisterOUTX_L_G_ | 0x80};
    uint8_t rx_read_buffer_[kTxRxReadLength_];

    Config config_;

    IBus::CommStatus writeRegister(uint8_t reg, uint8_t tx);
    IBus::CommStatus readRegister(uint8_t reg, uint8_t tx, uint8_t& rx);
};