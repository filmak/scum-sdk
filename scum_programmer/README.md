# SCuM Programmer

Load this firmware onto an nRF52840-DK to turn it into a programmer for SCuM!

The Single Chip micro-Mote (SCuM) is a 2x3mm2 single-chip standard-compatible Smart Dust chip, see https://www.crystalfree.org/

## Use

### program the nRF52840-DK

_Note_: you only need to do this once.

- download `scum-programmer.hex` from the https://github.com/openwsn-berkeley/SCuM-programmer/releases/latest/
- plug in your nRF52840-DK into your computer, drive "JLINK" appears
- drag-and-drop `scum-programmer.hex` onto this drive

### interact with SCuM's serial port

* Connect SCuM's UART to the following pins on the nRF52840-DK

| DK      | SCuM                     |
| ------- | ------------------------ |
| `P0.02` | UART TX (SCuM transmits) |
| `P0.26` | UART RX (SCuM receives)  |
| `GND`   | `GND`                    |

* open the serial port corresponding to your nRF52840-DK using a serial terminal (e.g. TeraTerm), using **19200 baud**.

### load code onto SCuM

The programmer.py script only takes firmware files in .bin format.
Use it as following:

```
programmer.py path/to/scum-firmware.bin
```

On Windows, the nRF J-Link TTY port cannot be detected automatically and needs
to be set manually using the `--port` option. For example:

```
programmer.py --port COM42 path/to/scum-firmware.bin
```

### calibrate SCuM

If the application requires calibration, use the `--calibrate` option to trigger
the calibration after booting SCuM:

```
programmer.py --calibrate path/to/scum-firmware.bin
```

## Build

- install SEGGER Embedded Studio for ARM (Nordic Edition)
- open `scum-programmer/scum-programmer.emProject`
