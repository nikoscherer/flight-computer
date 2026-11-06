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

    enum class ODR : uint8_t {
        kPowerDown = 0x00,
        k7_5Hz = 0x02,
        k960Hz = 0x09,
        k7_68kHz = 0x0D
    };

    enum class ODR_HG : uint8_t {
        kPowerDown = 0x00,
        k480Hz = 0x3,
        k960Hz = 0x04,
        k1_92kHz = 0x05,
        k3_84kHz = 0x06,
        k7_68kHz = 0x07
    };

    enum class FS_G : uint8_t {
        k250DPS = 0x01,
        k500DPS = 0x02,
        k1000DPS = 0x03,
        k2000DPS = 0x04,
        k4000DPS = 0x05
    };

    enum class FS_XL : uint8_t {
        k2G = 0x00,
        k4G = 0x01,
        k8G = 0x02,
        k16G = 0x03
    };

    enum class FS_XL_HG : uint8_t {
        k32G = 0x00,
        k64G = 0x01,
        k128G = 0x02,
        k256G = 0x03,
        k320G = 0x04
    };

    struct FullScaleConfig {
        uint8_t full_scale;
        float sensitivity;
    };

    DeviceStatus initialize() override;

    DeviceStatus read();
    void getReadRequest(SPIBusRequest& bus_request);

    void parseData(IMUData& imu_data);

    DeviceStatus checkWhoAmI();

    IBus::CommStatus setAccelFS(FS_XL accel_fs);
    IBus::CommStatus setGyroFS(FS_G gyro_fs);
    IBus::CommStatus setHGAccelFS(FS_XL_HG hg_accel_fs);

    IBus::CommStatus setAccelODR(ODR accel_odr);
    IBus::CommStatus setGyroODR(ODR gyro_odr);
    IBus::CommStatus setHGAccelODR(ODR_HG hg_accel_odr);

    constexpr FullScaleConfig getFullScaleConfig(FS_XL fs) {
        FullScaleConfig config;
        config.full_scale = static_cast<uint8_t>(fs);
        switch (fs) {
            case FS_XL::k2G:
                config.sensitivity = 0.061f;
                break;
            case FS_XL::k4G:
                config.sensitivity = 0.122f;
                break;
            case FS_XL::k8G:
                config.sensitivity = 0.244f;
                break;
            case FS_XL::k16G:
                config.sensitivity = 0.488f;
                break;
        }

        return config;
    }
    constexpr FullScaleConfig getFullScaleConfig(FS_XL_HG fs) {
        FullScaleConfig config;
        config.full_scale = static_cast<uint8_t>(fs);
        switch (fs) {
            case FS_XL_HG::k32G:
                config.sensitivity = 0.976f;
                break;
            case FS_XL_HG::k64G:
                config.sensitivity = 1.952f;
                break;
            case FS_XL_HG::k128G:
                config.sensitivity = 3.904f;
                break;
            case FS_XL_HG::k256G:
                config.sensitivity = 7.808f;
                break;
            case FS_XL_HG::k320G:
                config.sensitivity = 10.417f;
                break;
        }

        return config;
    }
    constexpr FullScaleConfig getFullScaleConfig(FS_G fs) {
        FullScaleConfig config;
        config.full_scale = static_cast<uint8_t>(fs);
        switch (fs) {
            case FS_G::k250DPS:
                config.sensitivity = 8.75f;
                break;
            case FS_G::k500DPS:
                config.sensitivity = 17.50f;
                break;
            case FS_G::k1000DPS:
                config.sensitivity = 35.0f;
                break;
            case FS_G::k2000DPS:
                config.sensitivity = 70.0f;
                break;
            case FS_G::k4000DPS:
                config.sensitivity = 140.0f;
                break;
        }

        return config;
    }

   private:
    IBus* bus_;

    GPIO_TypeDef* cs_gpio_port_;
    uint16_t cs_pin_;

    constexpr static uint32_t kTimeout_ = HAL_MAX_DELAY;

    constexpr static uint8_t kDeviceAddress_ = 0x73;

    constexpr static uint8_t kRegisterWHO_AM_I_ = 0x0F | 0x80,
                             kRegisterCTRL_1_ = 0x10, kRegisterCTRL_2_ = 0x11,
                             kRegisterCTRL_3_ = 0x12, kRegisterCTRL_6_ = 0x15,
                             kRegisterCTRL_8_ = 0x17,
                             kRegisterINT1_CTRL_ = 0x0D,
                             kRegisterCTRL_1_XL_HG_ = 0x4E,
                             // Read registers
        kRegisterOUTX_L_G_ = 0x22, kRegisterOUTX_L_A_ = 0x28,
                             kRegisterLengthOUT_L_A_ = 6,
                             kRegisterLengthOUT_L_G_ = 6;

    constexpr static uint8_t kTxRxReadLength_ =
        1 + kRegisterLengthOUT_L_G_ + kRegisterLengthOUT_L_A_;

    uint8_t tx_read_buffer_[kTxRxReadLength_] = {kRegisterOUTX_L_G_ | 0x80};
    uint8_t rx_read_buffer_[kTxRxReadLength_];

    FullScaleConfig accel_fs_config_;
    FullScaleConfig accel_hg_fs_config_;
    FullScaleConfig gyro_fs_config_;

    IBus::CommStatus writeRegister(uint8_t reg, uint8_t tx);
    IBus::CommStatus readRegister(uint8_t reg, uint8_t tx, uint8_t& rx);
};