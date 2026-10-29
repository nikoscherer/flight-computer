#include "imu.hpp"

#include "ibus.hpp"
#include "idevice.hpp"

IMU::IMU(IBus* bus, GPIO_TypeDef* cs_gpio_port, uint16_t cs_pin)
    : bus_(bus), cs_gpio_port_(cs_gpio_port), cs_pin_(cs_pin) {}

// We expect the bus to be free when we need to initialize
DeviceStatus IMU::initialize() {
    // WHO AM I
    uint8_t tx_who_am_i[2] = {kRegisterWHO_AM_I_, 0x00};
    uint8_t rx_who_am_i[2] = {0x00, 0x00};

    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    bus_->transmitReceive(tx_who_am_i, rx_who_am_i, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    if (rx_who_am_i[1] != kDeviceAddress) {
        return DeviceStatus::kInvalidDevice;
    }

    // CTRL1
    uint8_t tx_ctrl1[2] = {kRegisterCTRL_1_,
                           static_cast<uint8_t>(ODR::k7_68kHz)};  // ODR mode

    // uint8_t tx_ctrl1[2] = {0x10, 0x02};

    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_RESET);
    bus_->transmit(tx_ctrl1, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(cs_gpio_port_, cs_pin_, GPIO_PIN_SET);

    // CTRL3
    // BDU = 1, IF_INC = 1
    uint8_t tx_ctrl3[2] = {kRegisterCTRL_3_, 0x44};
    // uint8_t tx_ctrl3[2] = {0x12, 0x44};

    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    bus_->transmit(tx_ctrl3, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    // INT1_CTRL
    uint8_t tx_int1_ctrl[2] = {kRegisterINT1_CTRL_, 0x03};
    // uint8_t tx_int1_ctrl[2] = {0x0D, 0x03};

    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    bus_->transmit(tx_int1_ctrl, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    // Force read for EXTI callback
    uint8_t tx_dummy[7] = {kRegisterOUTX_L_A_ | 0x80};
    uint8_t rx_dummy[7] = {0};
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    bus_->transmitReceive(tx_dummy, rx_dummy, 7, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    return DeviceStatus::kOk;
}

DeviceStatus IMU::read() {
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    bus_->transmitReceiveDMA(tx_read_buffer_, rx_read_buffer_,
                             kRegisterLengthOUT_L_A);

    return DeviceStatus::kOk;
}

void IMU::parseData(IMUData& imu_data) {
    int16_t raw_x_accel =
        static_cast<int16_t>(rx_read_buffer_[2] << 8 | rx_read_buffer_[1]);
    int16_t raw_y_accel =
        static_cast<int16_t>(rx_read_buffer_[4] << 8 | rx_read_buffer_[3]);
    int16_t raw_z_accel =
        static_cast<int16_t>(rx_read_buffer_[6] << 8 | rx_read_buffer_[5]);

    imu_data.x_accel = (raw_x_accel * 0.061f / 1000.0f) * 9.80665f;
    imu_data.y_accel = (raw_y_accel * 0.061f / 1000.0f) * 9.80665f;
    imu_data.z_accel = (raw_z_accel * 0.061f / 1000.0f) * 9.80665f;
}