# **[Project](https://github.com/Fogotcheck/mqtt_exmp.git)**

## *Cloning the repository*

You can clone the repository as follows:

```console
git clone "https://github.com/Fogotcheck/mqtt_exmp.git"
```

## *Project structure*

```
├── AppMain                                 // AppMain.cpp
|
├── Cmake
|       ├── Toolchain                       // minimal set of build rules
|       ├── Opts                            // additional build functions
|       └── Utils                           // extra environment setup utilities
|
├── Chip                                    // platform-dependent libraries
|       └── STM32F439ZITx
|             ├── Core                      // chip core
|             ├── Drivers                   // HAL + CMSIS
|             └── STM32F439ZITx.ioc         // CubeMX auto-generation file
|
├── Lib                                     // project libraries
|       ├── ...
|       ├── FreeRTOS                        // example for building third-party libraries
|       └── CMakeLists.txt                  // CMakeLists for building libraries
|
|
├── CMakeLists.txt                          // main project build file
|
└── README.md                               // You are here

```

## *Project build*

### Project build setup
The project build is based on [CMake](https://cmake.org/). The minimum requirement for configuring the project build is specifying the toolchain:

```console
CMAKE_TOOLCHAIN_FILE=./Cmake/Toolchain/Arm-none-eabi-toolchain.cmake
```
Thus, the project setup for arm-none-eabi build will look like:

```console
cmake -DCMAKE_TOOLCHAIN_FILE=./Cmake/Toolchain/Arm-none-eabi-toolchain.cmake -B ./build
```

### Additional project setup utilities

[pre-commit](https://pre-commit.com) setup utility

```console
cmake --build ./build --target UTILS_BuildTest_VIRTUAL_ENV
```

Editor setup utility for [vscode](https://code.visualstudio.com/)
```console
cmake --build ./build --target UTILS_BuildTest_VSC_CONFIGURE
```
### Building the project

Build is performed with the command:

```console
cmake --build ./build
```

## [Mosquitto](https://mosquitto.org/)

mosquitto -c ./mosquitto/mosquitto.conf


## *Additional links*

* [Issue tracker](https://github.com/Fogotcheck/mqtt_exmp/issues)
