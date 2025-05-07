# SCuM Programmer

Use an nRF52840-DK and a Python script to program SCuM!

<p align="center">
  <img src="programmer-setup.svg" width="75%">
</p>

## Prerequisites

### Setup the nRF52840-DK

- Download `scum-programmer.hex` from the https://github.com/pisterlab/scum-sdk/releases/latest/
- Plug in your nRF52840-DK into your computer, drive "JLINK" appears
- Drag-and-drop `scum-programmer.hex` onto this drive

### Interact with SCuM's serial port

* Connect SCuM's UART to an USB-to-UART converter
* Open the serial port corresponding to your USB-to-UART converter using a
  serial terminal, using **19200 baud**.
  For example using [Socat](http://www.dest-unreach.org/socat/) on Linux:
  ```
  socat - open:/dev/ttyUSB0,b19200,echo=0,raw,cs8,parenb=0,cstopb=0
  ```

## Use

### Load code onto SCuM

The `main.py` script only takes firmware files in .bin format.
Use it as following:

```
main.py path/to/scum-firmware.bin
```

On Windows, the nRF J-Link TTY port cannot be detected automatically and needs
to be set manually using the `--port` option. For example:

```
main.py --port COM42 path/to/scum-firmware.bin
```

### Calibrate SCuM

If the application requires calibration, use the `--calibrate` option to trigger
the calibration after booting SCuM:

```
main.py --calibrate path/to/scum-firmware.bin
```

## Build the nRF firmware

- Install SEGGER Embedded Studio for ARM
- Open `scum_programmer/scum-programmer.emProject`
- Select the `scum-programmer` project and build it (F7)
- Download it on the nRF using the `Target > Download scum-programmer` menu
