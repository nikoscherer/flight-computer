#include "imu.hpp"

#include <cstdint>

#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"

#define g 9.80665f

IMU::IMU(IBus* bus, GPIO_TypeDef* cs_gpio_port, uint16_t cs_pin)
    : bus_(bus), cs_gpio_port_(cs_gpio_port), cs_pin_(cs_pin) {}

// We expect the bus to be free when we need to initialize
DeviceStatus IMU::initialize() {
    if (DeviceStatus status = checkWhoAmI();
        status == DeviceStatus::kInvalidDevice ||
        status == DeviceStatus::kError) {
        return status;
    }

    // Disale I2C and I3C
    uint8_t tx_if_cfg[2] = {kRegisterIF_CFG_, 0x01};

    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    bus_->transmit(tx_if_cfg, 2, kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    return DeviceStatus::kOk;
}

DeviceStatus IMU::setConfig(const IMU::Config& config) {
    // Accelerometer Config
    if (IBus::CommStatus status = setAccelFS(config.accel_config.fs_config.fs);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }
    if (IBus::CommStatus status = setAccelODR(config.accel_config.odr);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }

    // Hi-G Accelerometer Config

    // Gyroscope Config
    if (IBus::CommStatus status = setGyroFS(config.gyro_config.fs_config.fs);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }
    if (IBus::CommStatus status = setGyroODR(config.gyro_config.odr);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }

    // CTRL3 - Might want this changed
    // BDU = 1, IF_INC = 1
    if (IBus::CommStatus status = writeRegister(kRegisterCTRL_3_, 0x44);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }

    // Enable XL and G data ready interrupts
    if (IBus::CommStatus status = writeRegister(kRegisterINT1_CTRL_, 0x03);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }

    config_ = config;

    return DeviceStatus::kOk;
}

// Blocking
DeviceStatus IMU::read() {
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    bus_->transmitReceive(tx_read_buffer_, rx_read_buffer_,
                          1 + kRegisterLengthOUT_L_G_ + kRegisterLengthOUT_L_A_,
                          kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    return DeviceStatus::kOk;
}

void IMU::getReadRequest(SPIBusRequest& request) {
    BusRequest& bus_req = request.bus_request;
    bus_req.tx_buffer = tx_read_buffer_;
    bus_req.rx_buffer = rx_read_buffer_;
    bus_req.length = kTxRxReadLength_;

    bus_req.comm_type = BusRequestType::kTxRx;

    request.gpio_port = cs_gpio_port_;
    request.cs_pin = cs_pin_;

    request.bus_request = bus_req;
}

void IMU::parseData(IMUData& imu_data) {
    int16_t raw_pitch_vel =
        static_cast<int16_t>(rx_read_buffer_[2] << 8 | rx_read_buffer_[1]);
    int16_t raw_roll_vel =
        static_cast<int16_t>(rx_read_buffer_[4] << 8 | rx_read_buffer_[3]);
    int16_t raw_yaw_vel =
        static_cast<int16_t>(rx_read_buffer_[6] << 8 | rx_read_buffer_[5]);

    int16_t raw_x_accel =
        static_cast<int16_t>(rx_read_buffer_[8] << 8 | rx_read_buffer_[7]);
    int16_t raw_y_accel =
        static_cast<int16_t>(rx_read_buffer_[10] << 8 | rx_read_buffer_[9]);
    int16_t raw_z_accel =
        static_cast<int16_t>(rx_read_buffer_[12] << 8 | rx_read_buffer_[11]);

    float gyro_fs_mg_lsb = config_.gyro_config.fs_config.sens_mg_lsb;
    float accel_fs_mg_lsb = config_.accel_config.fs_config.sens_mg_lsb;

    imu_data.pitch_vel = raw_pitch_vel * gyro_fs_mg_lsb / 1000.0f;
    imu_data.roll_vel = raw_roll_vel * gyro_fs_mg_lsb / 1000.0f;
    imu_data.yaw_vel = raw_yaw_vel * gyro_fs_mg_lsb / 1000.0f;

    // (raw * mg/LSB (sensitivity)) * g
    imu_data.x_accel = (raw_x_accel * accel_fs_mg_lsb / 1000.0f) * g;
    imu_data.y_accel = (raw_y_accel * accel_fs_mg_lsb / 1000.0f) * g;
    imu_data.z_accel = (raw_z_accel * accel_fs_mg_lsb / 1000.0f) * g;
}

DeviceStatus IMU::checkWhoAmI() {
    uint8_t rx_address = 0x00;

    IBus::CommStatus status = readRegister(
        kRegisterWHO_AM_I_ | kReadRegisterPrefix_, 0x00, rx_address);

    if (status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }

    if (rx_address != kDeviceAddress_) {
        return DeviceStatus::kInvalidDevice;
    }

    return DeviceStatus::kOk;
}

IBus::CommStatus IMU::setAccelFS(AccelFS accel_fs) {
    uint8_t rx_ctrl8;

    IBus::CommStatus status =
        readRegister(kRegisterCTRL_8_ | kReadRegisterPrefix_, 0x00, rx_ctrl8);

    if (status != IBus::CommStatus::kOk) {
        return status;
    }

    uint8_t tx_ctrl8 = rx_ctrl8;

    // Clear the last 2 bits for FS_XL_ (and make sure bits 4-2 stay 0), and set
    // last 2 bits for FS_XL_
    tx_ctrl8 =
        (tx_ctrl8 & 0b11100000) | (static_cast<uint8_t>(accel_fs) & 0b00000011);

    return writeRegister(kRegisterCTRL_8_, tx_ctrl8);
}

IBus::CommStatus IMU::setAccelODR(AccelODR accel_odr) {
    uint8_t rx_ctrl1;

    IBus::CommStatus status = readRegister(kRegisterCTRL_1_, 0x00, rx_ctrl1);

    if (status != IBus::CommStatus::kOk) {
        return status;
    }

    uint8_t tx_ctrl1 = rx_ctrl1;

    // Clear the last 4 bits for ODR_XL_ (and make sure bit 7 stay 0), and set
    // last 4 bits for ODR_XL_
    tx_ctrl1 = (tx_ctrl1 & 0b01110000) |
               (static_cast<uint8_t>(accel_odr) & 0b00001111);

    return writeRegister(kRegisterCTRL_1_, tx_ctrl1);
}

IBus::CommStatus IMU::setHiGAccelFS(HiGAccelFS hg_accel_fs) {
    uint8_t rx_ctrl1_xl_hg;

    IBus::CommStatus status =
        readRegister(kRegisterCTRL_1_XL_HG_, 0x00, rx_ctrl1_xl_hg);

    if (status != IBus::CommStatus::kOk) {
        return status;
    }

    uint8_t tx_ctrl1_xl_hg = rx_ctrl1_xl_hg;

    tx_ctrl1_xl_hg = (tx_ctrl1_xl_hg & 0b11111000) |
                     (static_cast<uint8_t>(hg_accel_fs) & 0b00000111);

    return writeRegister(kRegisterCTRL_1_XL_HG_, tx_ctrl1_xl_hg);
}

IBus::CommStatus IMU::setHiGAccelODR(HiGAccelODR hg_accel_odr) {
    uint8_t rx_ctrl1_xl_hg;

    IBus::CommStatus status =
        readRegister(kRegisterCTRL_1_XL_HG_, 0x00, rx_ctrl1_xl_hg);

    if (status != IBus::CommStatus::kOk) {
        return status;
    }

    uint8_t tx_ctrl1_xl_hg = rx_ctrl1_xl_hg;

    tx_ctrl1_xl_hg = (tx_ctrl1_xl_hg & 0b11000111) |
                     (static_cast<uint8_t>(hg_accel_odr) & 0b00111000);

    return writeRegister(kRegisterCTRL_1_XL_HG_, tx_ctrl1_xl_hg);
}

IBus::CommStatus IMU::setGyroFS(GyroFS gyro_fs) {
    uint8_t rx_ctrl6;

    IBus::CommStatus status = readRegister(kRegisterCTRL_6_, 0x00, rx_ctrl6);

    if (status != IBus::CommStatus::kOk) {
        return status;
    }

    uint8_t tx_ctrl6 = rx_ctrl6;

    // bit 3 is explicitly 1
    tx_ctrl6 = (tx_ctrl6 & 0b01111000) | 0b00001000 |
               (static_cast<uint8_t>(gyro_fs) & 0b10000111);

    return writeRegister(kRegisterCTRL_6_, tx_ctrl6);
}

IBus::CommStatus IMU::setGyroODR(GyroODR gyro_odr) {
    uint8_t rx_ctrl2;

    IBus::CommStatus status = readRegister(kRegisterCTRL_2_, 0x00, rx_ctrl2);

    if (status != IBus::CommStatus::kOk) {
        return status;
    }

    uint8_t tx_ctrl2 = rx_ctrl2;

    tx_ctrl2 =
        (tx_ctrl2 & 0b01110000) | (static_cast<uint8_t>(gyro_odr) & 0b00001111);

    return writeRegister(kRegisterCTRL_2_, tx_ctrl2);
}

// Verify
IBus::CommStatus IMU::writeRegister(uint8_t reg, uint8_t tx) {
    uint8_t tx_buffer[2] = {reg, tx};
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    IBus::CommStatus status = bus_->transmit(tx_buffer, 2, kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    return status;
}

IBus::CommStatus IMU::readRegister(uint8_t reg, uint8_t tx, uint8_t& rx) {
    uint8_t tx_buffer[2] = {reg, tx};
    uint8_t rx_buffer[2];
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    IBus::CommStatus status =
        bus_->transmitReceive(tx_buffer, rx_buffer, 2, kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    if (status != IBus::CommStatus::kOk) {
        return IBus::CommStatus::kError;
    }

    rx = rx_buffer[1];

    return status;
}