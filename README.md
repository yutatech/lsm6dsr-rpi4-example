# lsm6dsr-rpl4-example
Example of communicating with the LSM6DSR (6-axis IMU) using a Raspberry Pi 4

## Build
```sh
cmake -S . -B build
cmake --build build --parallel
```

## Run
```sh
sudo ./build/
```