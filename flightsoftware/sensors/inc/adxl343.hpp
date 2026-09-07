#pragma once

#include <array>

#include "ibus.hpp"
#include "isensor.hpp"

class ADXL343 : public ISensor {
public:
  ADXL343(IBus *bus);
  ~ADXL343();

  struct ADXL343Data {
    int16_t x;
    int16_t y;
    int16_t z;
  };

  enum class PowerMode : uint8_t { // 0, 0, Link Bit, Auto Sleep Bit, Measure
                                   // Bit, Sleep Bit, Wakeup Bits
    kStandby = 0x00,
    kAwake = 0x08,
    kSleep = 0x04
  };

  enum class Range : uint8_t {
    k2G = 0x00,
    k4G = 0x01,
    k8G = 0x02,
    k16G = 0x03
  };

  constexpr static uint8_t kDataLength = 6; // 2 bytes for each axis (X, Y, Z)

  Status init() override;
  Status reset() override;

  Status readRaw(std::array<uint8_t, kDataLength> &out_raw_data);
  Status readRawDMA();
  Status setRange(const Range &range);
  Status setPowerMode(const PowerMode &mode);

  void parseRawData(ADXL343::ADXL343Data &out_data,
                    const std::array<uint8_t, kDataLength> &raw_data);

private:
  IBus *bus_;

  uint8_t rx_buffer_[kDataLength]; // Buffer for DMA reading

  constexpr static uint8_t kDeviceID_ = 0xE5;
  constexpr static uint8_t kI2CAddress_ = 0x53;

  constexpr static uint8_t kRegisterDEVID_ = 0x00, kRegisterTHRESH_TAP_ = 0x1D,
                           kRegisterOFSX_ = 0x1E, kRegisterOFSY_ = 0x1F,
                           kRegisterOFSZ_ = 0x20, kRegisterDUR_ = 0x21,
                           kRegisterLatent_ = 0x22, kRegisterWindow_ = 0x23,
                           kRegisterTHRESH_ACT_ = 0x24,
                           kRegisterTHRESH_INACT_ = 0x25,
                           kRegisterTIME_INACT_ = 0x26,
                           kRegisterACT_INACT_CTL_ = 0x27,
                           kRegisterTHRESH_FF_ = 0x28, kRegisterTIME_FF_ = 0x29,
                           kRegisterTAP_AXES_ = 0x2A,
                           kRegisterACT_TAP_STATUS_ = 0x2B,
                           kRegisterBW_RATE_ = 0x2C, kRegisterPOWER_CTL_ = 0x2D,
                           kRegisterINT_ENABLE_ = 0x2E,
                           kRegisterINT_MAP_ = 0x2F,
                           kRegisterINT_SOURCE_ = 0x30,
                           kRegisterDATA_FORMAT_ = 0x31,
                           kRegisterDATAX0_ = 0x32, kRegisterDATAX1_ = 0x33,
                           kRegisterDATAY0_ = 0x34, kRegisterDATAY1_ = 0x35,
                           kRegisterDATAZ0_ = 0x36, kRegisterDATAZ1_ = 0x37,
                           kRegisterFIFO_CTL_ = 0x38,
                           kRegisterFIFO_STATUS_ = 0x39;

  constexpr static uint16_t kTimeoutMs_ = 25;
};