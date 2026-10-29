#include "flightsoftware.hpp"

#include "main.h"

volatile bool initialized = false;
constexpr static uint8_t kRegisterOUTX_L_A_ = 0x28, kRegisterLengthOUT_L_A = 7;

uint8_t rx[2] = {0};
void fsw_init() {
    uint8_t tx[2] = {0x0F | 0x80, 0x00};
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    // CTRL1 (0x10)
    // ODR = 7.5 Hz, high-performance mode
    uint8_t tx_ctrl1[2] = {0x10, 0x02};

    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx_ctrl1, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    // CTRL3 (0x12):
    // BDU = 1, IF_INC = 1
    uint8_t tx_ctrl3[2] = {0x12, 0x44};

    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx_ctrl3, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    // INT1_CTRL (0x0D)
    uint8_t tx_int1_ctrl[2] = {0x0D, 0x03};

    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx_int1_ctrl, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    // To fix EXTI
    uint8_t tx_dummy[7] = {kRegisterOUTX_L_A_ | 0x80};
    uint8_t rx_dummy[7] = {0};
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx_dummy, rx_dummy, 7, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

    initialized = true;
}

struct IMUData {
    float x_accel;
    float y_accel;
    float z_accel;
};

IMUData imu_data;

int16_t raw_x_accel;
int16_t raw_y_accel;
int16_t raw_z_accel;

uint8_t tx_read_buffer_[10] = {kRegisterOUTX_L_A_ | 0x80};
uint8_t rx_read_buffer_[10];

bool data_ready = false;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == IMU_INT1_Pin) {
        data_ready = true;
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
    if (initialized && hspi->Instance == SPI1) {
        HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

        raw_x_accel =
            static_cast<int16_t>(rx_read_buffer_[2] << 8 | rx_read_buffer_[1]);
        raw_y_accel =
            static_cast<int16_t>(rx_read_buffer_[4] << 8 | rx_read_buffer_[3]);
        raw_z_accel =
            static_cast<int16_t>(rx_read_buffer_[6] << 8 | rx_read_buffer_[5]);

        imu_data.x_accel = (raw_x_accel * 0.061f / 1000.0f) * 9.80665f;
        imu_data.y_accel = (raw_y_accel * 0.061f / 1000.0f) * 9.80665f;
        imu_data.z_accel = (raw_z_accel * 0.061f / 1000.0f) * 9.80665f;
    }
}

bool status = false;
void fsw_loop() {
    status = HAL_GPIO_ReadPin(IMU_INT1_GPIO_Port, IMU_INT1_Pin);

    if (data_ready) {
        HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive_DMA(&hspi1, tx_read_buffer_, rx_read_buffer_,
                                    kRegisterLengthOUT_L_A);

        data_ready = false;
    }
}