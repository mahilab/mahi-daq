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

```cpp
// create and enable daq object
Q8Usb q8;
q8.enable();

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

// per module reads/writes
q8.AI.read();
q8.AO[0] = q8.AI[0];
q8.AO.write();

// per channel read/writes
di0 = q8.DI.read(0);
q8.DO.write(0,di0);

// automatic handling of shared pins
q8.PWM.set_channels({0,1,2,3}); // converts DO 0:3 to PWM outputs
q8.PWM.frequencies.write(0, 20000);
q8.PWM.write(0, 0.75);

// lightweight channel handles
DOHandle h_do0(q8.DO, 0);
AOHandle h_ao0(q8.AO, 0);
EncoderHandle h_enc0(q8.encoder, 0);
h_do0.write_level(LOW);
h_ao0.write_volts(0);
h_enc0.write_mode(QuadMode::X4);
h_enc0.set_units(360.0 / 1024);
h_enc0.zero();
MyRobot rob(h_do0, h_ao0, h_enc0);
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
