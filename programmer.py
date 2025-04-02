#!/usr/bin/env python

import random
import sys
import time
from dataclasses import dataclass

import click
import serial
from rich import print
from rich.console import Console
from serial.tools import list_ports
from tqdm import tqdm


def get_default_port():
    """Return default serial port."""
    ports = [port for port in list_ports.comports()]
    if sys.platform != "win32":
        ports = [port for port in ports if "J-Link" == port.product]
    if not ports:
        return "/dev/ttyACM0"
    # return first JLink port available
    return ports[0].device


# Constants
BINDATA_CHUNK_SIZE = 32

SERIAL_PORT_DEFAULT = get_default_port()
SERIAL_BAUDRATE_DEFAULT = 250000


@dataclass
class ScumProgrammerSettings:
    """Settings for the Scum Programmer."""

    port: str
    baudrate: int
    padding: str
    boot_mode: str
    firmware: str


class ScumProgrammer:
    """Class to handle the Scum Programmer."""

    def __init__(self, settings: ScumProgrammerSettings):
        """Initialize the Scum Programmer."""
        self.settings = settings
        self.console = Console()
        self.serial = serial.Serial(
            port=settings.port,
            baudrate=settings.baudrate,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=5,
        )

    def program(self):
        """Send firmware to nRF."""
        bindata = bytearray(open(self.settings.firmware, "rb").read())
        # Pad the firmware to 64kB
        code_length = len(bindata) - 1
        pad_length = 65536 - code_length - 1

        if self.settings.padding == "random":
            for _ in range(pad_length):
                bindata.append(random.randint(0, 255))
        else:
            for _ in range(pad_length):
                bindata.append(0)

        # Send the binary data over UART
        data_size = len(bindata)
        progress = tqdm(
            range(0, data_size),
            unit="B",
            unit_scale=False,
            colour="green",
            ncols=100,
        )
        progress.set_description(f"Loading firmware ({int(data_size / 1024)}kB)")
        pos = 0
        while (pos % BINDATA_CHUNK_SIZE) == 0 and pos < len(bindata):
            self.serial.write(bindata[pos : pos + BINDATA_CHUNK_SIZE])
            self.serial.flush()
            pos += BINDATA_CHUNK_SIZE
            progress.update(BINDATA_CHUNK_SIZE)
        progress.close()

        # Wait for response that writing is complete
        response = self.serial.read_until().decode().strip()
        if response != "OK":
            print(f"[bold red]Error: failed to load firmware to RAM[/]")
            sys.exit(1)

    def boot(self):
        """Boot the SCuM."""
        # Execute 3-wire bus bootloader on nRF
        print("[bold]Booting SCuM...[/]", end=" ")
        self.serial.write(f"{self.settings.boot_mode}\n".encode())
        # Wait for response that writing is complete
        response = self.serial.read_until().decode().strip()
        if response != "OK":
            print(f"[bold red]Error: failed to boot SCuM[/]")
            sys.exit(1)
        print("[bold green]:heavy_check_mark:[/]")

    def run(self):
        """Run the Scum Programmer."""
        print(f"[bold green]Starting Scum Programmer[/]")
        print(f"[bold]Padding type:[/] {self.settings.padding}")
        print(f"[bold]Boot mode:[/] {self.settings.boot_mode}")
        start = time.time()
        # Send firmware to nRF
        self.program()
        # Boot the SCuM
        self.boot()
        # Close serial port
        self.serial.close()
        print(f"[bold green]Done in {time.time() - start:.3f}s[/]")


@click.command(context_settings=dict(help_option_names=["-h", "--help"]))
@click.version_option("0.1.0", "-v", "--version")
@click.option(
    "-p",
    "--port",
    default=SERIAL_PORT_DEFAULT,
    help="Serial port to use for nRF.",
)
@click.option(
    "-b",
    "--baudrate",
    default=SERIAL_BAUDRATE_DEFAULT,
    help="Baudrate to use for nRF.",
)
@click.option(
    "-P",
    "--padding",
    type=click.Choice(["zero", "random"]),
    default="zero",
    help="Padding type used to fill the 64kB RAM.",
)
@click.option(
    "-b",
    "--boot-mode",
    type=click.Choice(["3wb", "optical"]),
    default="3wb",
    help="Boot mode to use.",
)
@click.argument("firmware", type=click.File(mode="rb"), required=True)
def main(port, baudrate, padding, boot_mode, firmware):
    programmer_settings = ScumProgrammerSettings(
        port=port,
        baudrate=baudrate,
        padding=padding,
        boot_mode=boot_mode,
        firmware=firmware.name,
    )
    programmer = ScumProgrammer(programmer_settings)
    programmer.run()


if __name__ == "__main__":
    main()
