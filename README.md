# A proprietary free SCuM SDK

[![CI][ci-badge]][ci-link]
[![License][license-badge]][license-link]

This repository is an attempt to provide a proprietary free support for the
Single Chip micro-Mote (SCuM).
The SCuM chip is a 2x3mm2 single-chip standard-compatible Smart Dust chip,
see https://www.crystalfree.org/.
Most of the drivers (UART, Radio, Timer, SPI) are taken from the
[scum-test-code](https://github.com/PisterLab/scum-test-code) repository.

## Requirements

What is needed to build a firmware for SCuM:
- [CMake](https://cmake.org/)
- [Ninja](https://ninja-build.org/)
- [GNU ARM Embedded toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

Make sure that the ARM toolchain GCC program is available in your PATH.

## Get the code

Use Git:
```
git clone https://github.com/PisterLab/scum-sdk.git
```

## Build a SCuM firmware

Some sample applications are provided in the [sdk/samples](sdk/samples) directory.
To build an application, from the repository base directory, simply run
(for Windows users adapt the path separators to "\"):

```
cmake -S sdk/samples/hello_world -B sdk/samples/hello_world/build -GNinja -DCMAKE_BUILD_TYPE=MinSizeRel
ninja -C sdk/samples/hello_world/build
```

The generated firmwares (elf, hex, bin) are located in the `sdk/samples/hello_world/build` directory.

## Program SCuM

Once the SCuM chip is properly connected to an nRF52840-DK programmer, use the
SCuM programmer `main.py` script available in the [scum_programmer](scum_programmer) directory.

The build system also proposes a `load` target to automatically call the SCuM programmer
script:

```
ninja -C sdk/samples/hello_world/build load
```

## Contribute

Check out the [contributing guide](CONTRIBUTING.md) to see how to participate
to the SCuM project.

## License

The SCuM SDK is published under the [BSD 3-clauses license](LICENSE.txt).

[ci-badge]: https://github.com/pisterlab/scum-sdk/workflows/CI/badge.svg
[ci-link]: https://github.com/pisterlab/scum-sdk/actions?query=workflow%3ACI+branch%3Amain
[license-badge]: https://img.shields.io/github/license/pisterlab/scum-sdk
[license-link]: https://github.com/pisterlab/scum-sdk/blob/main/LICENSE.txt
