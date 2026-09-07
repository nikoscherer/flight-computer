#include "adxl343.hpp"

#include <array>
#include <cstdint>

#include "ibus.hpp"

ADXL343::ADXL343(IBus *bus) : bus_(bus) {}
ADXL343::~ADXL343() = default;

ADXL343::Status ADXL343::init() {
  // Read device ID from the sensor
  uint8_t device_id{};
  IBus::Status dev_id_status =
      bus_->readRegister(kRegisterDEVID_, &device_id, 1, kTimeoutMs_);
  if (dev_id_status != IBus::Status::kOk) {
    return Status::kBusReadError;
  }

  // Check device ID against expected value
  if (device_id != kDeviceID_) {
    return Status::kInvalidID;
  }

  // Sets power mode to awake after initialization
  ISensor::Status pwr_status = setPowerMode(PowerMode::kAwake);
  return pwr_status;
}

// Non-blocking reset? If we have to reset during flight?
ADXL343::Status ADXL343::reset() {
  return Status::kError;
} // Return error because it's not implemented

ADXL343::Status
ADXL343::readRaw(std::array<uint8_t, kDataLength> &out_raw_data) {
  IBus::Status rx_status = // Read 6 bytes of data from the sensor (2 bytes
                           // each axis)
      bus_->readRegister(kRegisterDATAX0_, out_raw_data.data(), kDataLength,
                         kTimeoutMs_);
  if (rx_status != IBus::Status::kOk) {
    return Status::kBusReadError;
  }

  return Status::kOk;
}

ADXL343::Status ADXL343::readRawDMA() {
  IBus::Status rx_status = // Read 6 bytes of data from the sensor (2 bytes for
                           // each axis)
      bus_->readRegisterDMA(kRegisterDATAX0_, rx_buffer_, kDataLength);

  return (rx_status == IBus::Status::kOk)
             ? Status::kOk
             : Status::kBusReadError; // If rx_status is ok, return kOk, else
                                      // return kBusReadError
}

// Turn raw uint8_t array to ADXL343Data
void ADXL343::parseRawData(ADXL343::ADXL343Data &out_data,
                           const std::array<uint8_t, kDataLength> &raw_data) {
  out_data.x = static_cast<int16_t>((raw_data[1] << 8) | raw_data[0]);
  out_data.y = static_cast<int16_t>((raw_data[3] << 8) | raw_data[2]);
  out_data.z = static_cast<int16_t>((raw_data[5] << 8) | raw_data[4]);
}

ADXL343::Status ADXL343::setRange(const Range &range) {
  uint8_t range_val = static_cast<uint8_t>(range); // Value of range to be set
  IBus::Status rx_status =
      bus_->writeRegister(kRegisterDATA_FORMAT_, &range_val, 1, kTimeoutMs_);
  if (rx_status != IBus::Status::kOk) {
    return Status::kBusWriteError;
  }
  return Status::kOk;
}

ADXL343::Status ADXL343::setPowerMode(const PowerMode &mode) {
  uint8_t rx_pwr_ctrl{}; // Get power control register bits
  IBus::Status rx_status =
      bus_->readRegister(kRegisterPOWER_CTL_, &rx_pwr_ctrl, 1, kTimeoutMs_);
  if (rx_status != IBus::Status::kOk) {
    return Status::kBusReadError;
  }

  PowerMode current_mode = static_cast<PowerMode>(
      rx_pwr_ctrl); // value of current power mode value to power mode
  if (current_mode == mode) {
    return Status::kOk; // Already in the desired power mode
  }

  // Values for the different power modes to be set in the POWER_CTL register
  uint8_t awake_mode_val = static_cast<uint8_t>(PowerMode::kAwake);
  uint8_t standby_mode_val = static_cast<uint8_t>(PowerMode::kStandby);
  uint8_t sleep_mode_val = static_cast<uint8_t>(PowerMode::kSleep);
  switch (mode) {
  case PowerMode::kStandby: {
    // Set the power mode to standby
    IBus::Status rx_standby_status = bus_->writeRegister(
        kRegisterPOWER_CTL_, &standby_mode_val, 1, kTimeoutMs_);
    if (rx_standby_status != IBus::Status::kOk) {
      return Status::kBusWriteError;
    }
    break;
  }
  case PowerMode::kAwake: { // Setting the power mode to awake requires first
                            // checking if the sensor is in sleep mode. If it
                            // is, we need to set it to standby first before
                            // setting it to awake.
    // Set the power mode to standby
    if (current_mode == PowerMode::kSleep) {
      // If currently in sleep mode, wake up the sensor first
      IBus::Status rx_standby_status = bus_->writeRegister(
          kRegisterPOWER_CTL_, &standby_mode_val, 1, kTimeoutMs_);
      if (rx_standby_status != IBus::Status::kOk) {
        return Status::kBusWriteError;
      }
    }

    // Set the power mode to awake
    IBus::Status rx_awake_status = bus_->writeRegister(
        kRegisterPOWER_CTL_, &awake_mode_val, 1, kTimeoutMs_);
    if (rx_awake_status != IBus::Status::kOk) {
      return Status::kBusWriteError;
    }

    break;
  }
  case PowerMode::kSleep: {
    // Set the power mode to sleep
    IBus::Status rx_sleep_status = bus_->writeRegister(
        kRegisterPOWER_CTL_, &sleep_mode_val, 1, kTimeoutMs_);
    if (rx_sleep_status != IBus::Status::kOk) {
      return Status::kBusWriteError;
    }
    break;
  }
  default:
    return Status::kError; // Invalid power mode, check code.
  }

  return Status::kOk;
}
