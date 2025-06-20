#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "lsm6dsr_rpl4_lib/lsm6dsr.h"
#include "rpl4/peripheral/gpio.hpp"
#include "rpl4/peripheral/spi.hpp"
#include "rpl4/rpl4.hpp"

void SpiSetup(std::shared_ptr<rpl::Spi> spi) {

  // GPIO configuration
  rpl::Gpio::SetAltFunction(7, rpl::Gpio::AltFunction::kAlt0);   // SPI0_CE1
  rpl::Gpio::SetAltFunction(8, rpl::Gpio::AltFunction::kAlt0);   // SPI0_CE0
  rpl::Gpio::SetAltFunction(9, rpl::Gpio::AltFunction::kAlt0);   // SPI0_MISO
  rpl::Gpio::SetAltFunction(10, rpl::Gpio::AltFunction::kAlt0);  // SPI0_MOSI
  rpl::Gpio::SetAltFunction(11, rpl::Gpio::AltFunction::kAlt0);  // SPI0_SCLK

  // SPI0_CE1
  rpl::Gpio::SetPullRegister(7, rpl::Gpio::PullRegister::kNoRegister);
  // SPI0_CE0
  rpl::Gpio::SetPullRegister(8, rpl::Gpio::PullRegister::kPullUp);
  // SPI0_MISO
  rpl::Gpio::SetPullRegister(9, rpl::Gpio::PullRegister::kPullDown);
  // SPI0_MOSI
  rpl::Gpio::SetPullRegister(10, rpl::Gpio::PullRegister::kPullDown);
  // SPI0_SCLK
  rpl::Gpio::SetPullRegister(11, rpl::Gpio::PullRegister::kPullDown);

  // SPI configuration
  spi->SetClockPhase(rpl::Spi::ClockPhase::kMiddle);
  spi->SetClockPolarity(rpl::Spi::ClockPolarity::kHigh);
  spi->SetClockDivider(1024);
  spi->SetCs1Polarity(rpl::Spi::CsPolarity::kLow);
  spi->SetCs0Polarity(rpl::Spi::CsPolarity::kLow);
  spi->SetReadEnable(rpl::Spi::ReadEnable::kDisable);
}

int main() {
  using namespace lsm6dsr_rpl4_lib;
  using namespace std::chrono_literals;

  rpl::Init();

  std::shared_ptr<rpl::Spi> spi = rpl::Spi::GetInstance(rpl::Spi::Port::kSpi0);
  SpiSetup(spi);

  std::this_thread::sleep_for(100ms);  // Wait for SPI setup

  LSM6DSR lsm6dsr(spi, rpl::Spi::ChipSelect::kChipSelect1);
  lsm6dsr.Init();
  uint8_t data = 0b10000001;
  // lsm6dsr.WriteRegister(Register::CTRL3_C, &data);  // 104Hz, 2g
  lsm6dsr.WriteI3cEnabled(LSM6DSR::EnableState::kDisabled);
  // lsm6dsr.WriteAutoIncrementEnabled(LSM6DSR::EnableState::kEnabled);
  // lsm6dsr.WriteGyroDataRate(LSM6DSR::GyroDataRate::k6660Hz);
  // lsm6dsr.WriteAccDataRate(LSM6DSR::AccelDataRate::k6660Hz);
  // lsm6dsr.WriteGyroFullScale(LSM6DSR::GyroFullScale::k2000dps);
  // lsm6dsr.ConfigureGyroSensitivity(LSM6DSR::GyroFullScale::k2000dps);
  // lsm6dsr.WriteAccFullScale(LSM6DSR::AccelFullScale::k16g);
  // lsm6dsr.ConfigureAccSensitivity(LSM6DSR::AccelFullScale::k16g);
  std::this_thread::sleep_for(100ms);  // Wait for SPI setup

  lsm6dsr.WhoAmI();
  return 0;

  float gyro_x, gyro_y, gyro_z;
  float acc_x, acc_y, acc_z;
  while (true) {
    if(lsm6dsr.ReadAccAndGyro(gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z)) {
      printf("Gyro: X=%+08.2f Y=%+08.2f Z=%+08.2f | ", gyro_x, gyro_y, gyro_z);
      printf("Accel: X=%+08.2f Y=%+08.2f Z=%+08.2f\n", acc_x, acc_y, acc_z);
    } else {
      std::cerr << "Failed to read acceleration data!" << std::endl;
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1));  // 100 ms delay
  }

  return 0;
}