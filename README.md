<p align="center">
<img src="https://github.com/mahilab/mahi-daq/blob/master/mahi-daq.png" width="256"> 
</p>

## mahi-daq

This library provides a unfied hardware abstraction layer for commercial multifunction DAQs from Quanser, Sensoray, and National Instruments. It can be used for anything ranging from taking simple samples, to complex real-time controls. 

### Integration

The library is small and intended to be used with CMake's `FetchContent`:

```cmake
include(FetchContent) 
FetchContent_Declare(mahi-daq GIT_REPOSITORY https://github.com/mahilab/mahi-daq.git) 
FetchContent_MakeAvailable(mahi-daq)

add_executable(my_app "my_app.cpp")
target_link_libraries(my_app mahi::daq)
```

That's it! You should also be able to install or use the library as a git-submodule + CMake subdirectory if you prefer.

### Currently Supported Devices

- Quanser Q8-USB 
- Quanser QPID
- Quanser Q2
- Sensoray S826
- NI myRIO

### Example Usage 

#### Opening and Configuration
```cpp
// open Q8-USB
Q8Usb q8;
// create DAQ specific options
QuanserOptions opts;
opts.encX_filter[0] = QuanserOptions::EncoderFilter::Filtered;
opts.encX_velocity[0] = 0.1;
opts.aoX_params[0].ch_mode = QuanserOptions::AoMode::CurrentMode1;
opts.aoX_params[0].ch_kff = 0;
opts.aoX_params[0].ch_a0 = 2;
opts.aoX_params[0].ch_a1 = 20;
opts.aoX_params[0].ch_a2 = 0;
opts.aoX_params[0].ch_b0 = -1;
opts.aoX_params[0].ch_b1 = 0;
opts.aoX_params[0].ch_post = 1000;
// set options
q8.set_options(opts);
```
#### Synchronized Reads/Writes
```cpp
// synchronized reads
q8.read_all();
bool di0 = q8.DI[0];
double ai5 = q8.AI[5];
int enc3 = q8.encoder[3];
double vel2 = q8.velocity[2];
// synchronized writes
q8.DO[7] = true;
q8.AO[2] = 1.23;
q8.write_all();
```
#### Per Module Reads/Writes
```cpp
q8.AI.read();
q8.AO[0] = q8.AI[7];
q8.AO.write();
```
#### Per Channel Reads/Writes
```cpp
q8.DI.read(1);
q8.DO.write(1,q8.DI[1]);
```
#### Set Enable/Disable/Expire Values
```cpp
q8.DO.enable_values[0] = true;
q8.AO.enable_values[0] = 3.14;
q8.AO.disable_values[0] = 0;
q8.AO.expire_values.write(0,0);
...
q8.enable(); // AO0 will go to 3.14 V, DO0 will go to 5 V
...
q8.disabe(); // AO0 will go to 0 V, DO0 will go to 0 V (default)
```
#### Watchdog Support
```cpp
q8.watchdog.set_timeout(10_ms);
q8.watchdog.start();
while (true) {
    ...
    q8.watchdog.kick();
}
q8.watchdog.stop();
```
#### Automatic Handling of Shared Pins
```cpp
q8.PWM.set_channels({0,1,2,3}); // converts DO 0:3 to PWM outputs
q8.PWM.frequencies.write(0, 20000);
q8.PWM.write(0, 0.75);
```
#### Lightweight Handles for Individual Channels
```cpp
DOHandle h_do0(q8.DO, 0);
AOHandle h_ao0(q8.AO, 0);
EncoderHandle h_enc0(q8.encoder, 0);
// I/O through handles
h_do0.write_level(TTL_LOW);
h_ao0.write_volts(0);
h_enc0.write_mode(QuadMode::X4);
h_enc0.set_units(360.0 / 1024);
h_enc0.zero();
// pass handles to other classes
MyRobot rob(h_do0, h_ao0, h_enc0);
```
#### Precision Timing Utilities
```cpp
Q8Usb q8;
Timer timer(1000_Hz);
Time t = Time::Zero;
while (t < 60_s) {
    q8.read_all();
    my_controller_update(t, q8.AO[0],q8.AI[0],q8.encoder[0]);
    q8.write_all();
    t = timer.wait();
}
```

### Requirments 

- C++11 compiler
- Quanser only: [Quanser HIL SDK](https://github.com/quanser/hil_sdk_win64)
- Sensoray S826 only: [S826 SDK](http://www.sensoray.com/PCI_Express_digital_output_826.htm)
- myRIO only: [GNU C & C++ Compilers for ARMv7 Linux](http://www.ni.com/download/labview-real-time-module-2018/7813/en/)

### Building for Windows DAQs (Quanser/Sensoray)

On Windows, we recommend using to MSVC 2019:

```shell
> cd mahi-daq
> mkdir build
> cmake .. -G "Visual Studio 16 2019" -A x64
> cmake --build . --config Release
```

`mahi-daq` will automatically scan your system for supported SDKs, and build the associated DAQ classes.

### Building for myRIO

First, download and install the [NI ARM cross-compiler](http://www.ni.com/download/labview-real-time-module-2018/7813/en/) to `C:/dev/nilrt-arm` so that the directory contains `sysroots/`, `relocate_sdk.py`, etc. 

`mahi-daq` ships with a CMake toolchain file, which can be used like so:

```shell
> cd mahi-daq
> mkdir build
> cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE="../cmake/nilrt-arm-toolchain.cmake" -DCMAKE_BUILD_TYPE="Release"
> cmake --build .
```

Now, you can transfer the compiled example binaries from `mahi-daq/build/examples` over SFTP and run them though SSH.

```shell
# terminal 1
> cd examples
> sftp admin@172.22.11.2
> put myrio
> chmod 777 myrio
```
```shell
# terminal 2
> ssh admin@172.22.11.2
> ./myrio
```
