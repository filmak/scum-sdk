# A proprietary free SCuM SDK

This repository is an attempt to provide a proprietary free support for the
SCuM chip.
Most of the drivers (UART, Radio, Timer, SPI) are taken from the
[scum-test-code](https://github.com/PisterLab/scum-test-code) repository.

### Requirements

So far this repository has only been tested on Linux.

What is needed to build a firmware for SCuM:
- GNU Make
- [GNU ARM Embedded toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

Make sure that the ARM toolchain is available in your PATH.

### Get the code

Use Git:
```
git clone https://github.com/aabadie/scum-sdk.git
```

### Usage

So far, only the original `hello_world` example application is available. To
build this application, from the repository base directory, simply run:

```
make -C samples/hello_world
```

The generated firmware are located in the `samples/hello_world/build` directory.

### Flash the firmware

Once the SCuM chip is properly connected to an nRF52840-DK programmer, use the
SCuM programmer script available at https://github.com/aabadie/SCuM-programmer/tree/develop_12_refactor:

```
<path to scum_programmer>/programmer.py samples/hello_world/build/hello_world.bin
```
