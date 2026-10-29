// #include "imu.hpp"

// #include <cstdint>

// #include "ibus.hpp"

// IMU::IMU(CommBusType bus_type, int* cs_gpio_port, uint16_t cs_pin)
//     : bus_type_(bus_type), cs_gpio_port_(cs_gpio_port), cs_pin_(cs_pin) {}

// void IMU::requestRead(BusRequest& request, uint32_t timeout) {
//     request.tx_buffer = tx_read_buffer_;
//     request.rx_buffer = rx_read_buffer_;
//     request.length = kRegisterLengthOUT_L_A;
//     request.timeout = timeout;

//     request.cs_gpio_port = cs_gpio_port_;
//     request.cs_pin = cs_pin_;

//     request.comm_type = BusRequestType::kTxRx;
//     request.bus_type = bus_type_;
// }

// void IMU::getAccel(IMUData& imu_data) {
//     int16_t raw_x_accel =
//         static_cast<int16_t>(rx_read_buffer_[2] << 8 | rx_read_buffer_[1]);
//     int16_t raw_y_accel =
//         static_cast<int16_t>(rx_read_buffer_[4] << 8 | rx_read_buffer_[3]);
//     int16_t raw_z_accel =
//         static_cast<int16_t>(rx_read_buffer_[6] << 8 | rx_read_buffer_[5]);

//     // Note, 0.061f is the mg/LSB of the data, depends on FS (CTRL6 register)
//     imu_data.x_accel = (raw_x_accel * 0.061f / 1000.0f) * 9.80665f;
//     imu_data.y_accel = (raw_y_accel * 0.061f / 1000.0f) * 9.80665f;
//     imu_data.z_accel = (raw_z_accel * 0.061f / 1000.0f) * 9.80665f;
// }