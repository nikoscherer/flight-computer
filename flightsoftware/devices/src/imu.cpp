#include "imu.hpp"

#include "ibus.hpp"
#include "idevice.hpp"
#include "main.h"

IMU::IMU(IBus* bus, GPIO_TypeDef* cs_gpio_port, uint16_t cs_pin)
    : bus_(bus), cs_gpio_port_(cs_gpio_port), cs_pin_(cs_pin) {}

// We expect the bus to be free when we need to initialize
DeviceStatus IMU::initialize() {
    // Doesnt work idk why
    if (DeviceStatus status = checkWhoAmI();
        status == DeviceStatus::kInvalidDevice) {
        return status;
    }

    // Set IMU FS
    if (IBus::CommStatus status = setAccelFS(FS_XL::k4G);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }
    if (IBus::CommStatus status = setGyroFS(FS_G::k1000DPS);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }

    // // Set IMU ODR
    if (IBus::CommStatus status = setAccelODR(ODR::k7_68kHz);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }
    if (IBus::CommStatus status = setGyroODR(ODR::k7_68kHz);
        status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }

    // CTRL3 - Might want this changed
    // BDU = 1, IF_INC = 1
    // uint8_t tx_ctrl3[2] = {kRegisterCTRL_3_, 0x44};

    // HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    // bus_->transmit(tx_ctrl3, 2, kTimeout_);
    // HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    // // INT1_CTRL
    uint8_t tx_int1_ctrl[2] = {kRegisterINT1_CTRL_, 0x03};

    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    bus_->transmit(tx_int1_ctrl, 2, kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    return DeviceStatus::kOk;
}

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
    bus_req.length =
        1 + kRegisterLengthOUT_L_G_ +
        kRegisterLengthOUT_L_A_;  // bus_req.length = kRegisterLengthOUT_L_A;

    bus_req.comm_type = BusRequestType::kTxRx;

    request.gpio_port = cs_gpio_port_;
    request.cs_pin = cs_pin_;

    request.bus_request = bus_req;
}

// Need to work for different fs
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

    imu_data.pitch_vel = raw_pitch_vel * gyro_fs_config_.sensitivity / 1000.0f;
    imu_data.roll_vel = raw_roll_vel * gyro_fs_config_.sensitivity / 1000.0f;
    imu_data.yaw_vel = raw_yaw_vel * gyro_fs_config_.sensitivity / 1000.0f;

    // (raw * mg/LSB (sensitivity)) * g
    imu_data.x_accel =
        (raw_x_accel * accel_fs_config_.sensitivity / 1000.0f) * 9.80665f;
    imu_data.y_accel =
        (raw_y_accel * accel_fs_config_.sensitivity / 1000.0f) * 9.80665f;
    imu_data.z_accel =
        (raw_z_accel * accel_fs_config_.sensitivity / 1000.0f) * 9.80665f;
}

// WHO AM I
DeviceStatus IMU::checkWhoAmI() {
    uint8_t read_address = 0x00;

    IBus::CommStatus status =
        readRegister(kRegisterWHO_AM_I_ | 0x80, 0x00, read_address);

    if (status != IBus::CommStatus::kOk) {
        return DeviceStatus::kError;
    }

    if (read_address != kDeviceAddress_) {
        return DeviceStatus::kInvalidDevice;
    }

    return DeviceStatus::kOk;
}

// FS
IBus::CommStatus IMU::setAccelFS(FS_XL accel_fs) {
    IBus::CommStatus status =
        writeRegister(kRegisterCTRL_8_, static_cast<uint8_t>(accel_fs));

    if (status == IBus::CommStatus::kOk) {
        accel_fs_config_ = getFullScaleConfig(accel_fs);
    }

    return status;
}

// Not implemented
IBus::CommStatus IMU::setHGAccelFS(FS_XL_HG hg_accel_fs) {
    uint8_t current = 0x00;
    readRegister(kRegisterCTRL_2_ | 0x80, 0x00, current);

    current &= 0x07 >> 3;
    current |= static_cast<uint8_t>(hg_accel_fs);

    return IBus::CommStatus::kError;
}

IBus::CommStatus IMU::setGyroFS(FS_G gyro_fs) {
    IBus::CommStatus status =
        writeRegister(kRegisterCTRL_6_, static_cast<uint8_t>(gyro_fs));
    if (status == IBus::CommStatus::kOk) {
        gyro_fs_config_ = getFullScaleConfig(gyro_fs);
    }

    return status;
}

// ODR
IBus::CommStatus IMU::setAccelODR(ODR accel_odr) {
    uint8_t tx_ctrl1[2] = {kRegisterCTRL_1_, static_cast<uint8_t>(accel_odr)};

    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    IBus::CommStatus status =
        bus_->transmit(tx_ctrl1, sizeof(tx_ctrl1), kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    return status;
}
IBus::CommStatus IMU::setGyroODR(ODR gyro_odr) {
    uint8_t tx_ctrl2[2] = {kRegisterCTRL_2_, static_cast<uint8_t>(gyro_odr)};

    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    IBus::CommStatus status =
        bus_->transmit(tx_ctrl2, sizeof(tx_ctrl2), kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    return status;
}
IBus::CommStatus IMU::setHGAccelODR(ODR_HG hg_accel_odr) {
    // Read register first
    uint8_t tx_ctrl1_xl_hg[2] = {kRegisterCTRL_1_XL_HG_ |
                                 0x80};  // 0x80 to read
    uint8_t rx_ctrl1_xl_hg[2];

    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    IBus::CommStatus status = bus_->transmitReceive(
        tx_ctrl1_xl_hg, rx_ctrl1_xl_hg, sizeof(rx_ctrl1_xl_hg), kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    tx_ctrl1_xl_hg[0] = kRegisterCTRL_1_XL_HG_;
    tx_ctrl1_xl_hg[1] =
        (rx_ctrl1_xl_hg[1] & ~(0x1C)) | (static_cast<uint8_t>(hg_accel_odr));

    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    status = bus_->transmit(tx_ctrl1_xl_hg, sizeof(tx_ctrl1_xl_hg), kTimeout_);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    return status;
}

// Implement
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

    rx = rx_buffer[1];

    return status;
}