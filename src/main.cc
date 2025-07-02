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
  rpl::Gpio::SetAltFunction(9, rpl::Gpio::AltFunction::kAlt0);   // SPI0_MISO
  rpl::Gpio::SetAltFunction(10, rpl::Gpio::AltFunction::kAlt0);  // SPI0_MOSI
  rpl::Gpio::SetAltFunction(11, rpl::Gpio::AltFunction::kAlt0);  // SPI0_SCLK

  // SPI0_MISO
  rpl::Gpio::SetPullRegister(9, rpl::Gpio::PullRegister::kNoRegister);
  // SPI0_MOSI
  rpl::Gpio::SetPullRegister(10, rpl::Gpio::PullRegister::kNoRegister);
  // SPI0_SCLK
  rpl::Gpio::SetPullRegister(11, rpl::Gpio::PullRegister::kNoRegister);

  // SPI configuration
  spi->SetClockPhase(rpl::Spi::ClockPhase::kMiddle);
  spi->SetClockPolarity(rpl::Spi::ClockPolarity::kHigh);
  spi->SetClockDivider(32);
  spi->SetCs1Polarity(rpl::Spi::CsPolarity::kLow);
  spi->SetCs0Polarity(rpl::Spi::CsPolarity::kLow);
  spi->SetReadEnable(rpl::Spi::ReadEnable::kDisable);
}

int main() {
  using namespace lsm6dsr_rpl4_lib;
  using namespace std::chrono_literals;

  rpl::Init();

  std::shared_ptr<rpl::Spi> spi = rpl::Spi::GetInstance(rpl::Spi::Port::kSpi0);
  std::shared_ptr<rpl::Gpio> gpio = rpl::Gpio::GetInstance(7);
  SpiSetup(spi);

  LSM6DSR lsm6dsr(spi, rpl::Spi::ChipSelect::kChipSelect1, gpio);
  lsm6dsr.ResetMemory();
  std::this_thread::sleep_for(15ms);
  lsm6dsr.RebootDevice();
  std::this_thread::sleep_for(50us);
  lsm6dsr.Init();
  lsm6dsr.WriteGyroDataRate(LSM6DSR::GyroDataRate::k6660Hz);
  lsm6dsr.WriteAccDataRate(LSM6DSR::AccelDataRate::k6660Hz);
  lsm6dsr.WriteGyroFullScale(LSM6DSR::GyroFullScale::k1000dps);
  lsm6dsr.ConfigureGyroSensitivity(LSM6DSR::GyroFullScale::k1000dps);
  lsm6dsr.WriteAccFullScale(LSM6DSR::AccelFullScale::k8g);
  lsm6dsr.ConfigureAccSensitivity(LSM6DSR::AccelFullScale::k8g);

  lsm6dsr.WhoAmI();

  float gyro_x, gyro_y, gyro_z;
  float acc_x, acc_y, acc_z;
  while (true) {
    if (lsm6dsr.ReadAccAndGyro(gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z)) {
      printf("Gyro: X=%+08.2f Y=%+08.2f Z=%+08.2f | ", gyro_x, gyro_y, gyro_z);
      printf("Accel: X=%+08.2f Y=%+08.2f Z=%+08.2f\n", acc_x, acc_y, acc_z);
    } else {
      std::cerr << "Failed to read acceleration data!" << std::endl;
    }
    std::this_thread::sleep_for(10ms);  // 10 ms delay
  }

  return 0;
}
