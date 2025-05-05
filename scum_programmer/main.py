#!/usr/bin/env python

"""SCuM Programmer CLI."""

import click

try:
    from scum_programmer.programmer import __version__
    from scum_programmer.programmer.scum import ScumProgrammer, ScumProgrammerSettings
    from scum_programmer.programmer.serial import get_default_port
except ImportError:
    from programmer import __version__
    from programmer.scum import ScumProgrammer, ScumProgrammerSettings
    from programmer.serial import get_default_port


SERIAL_PORT_DEFAULT = get_default_port()
SERIAL_BAUDRATE_DEFAULT = 460800


@click.command(context_settings=dict(help_option_names=["-h", "--help"]))
@click.version_option(__version__, "-v", "--version", prog_name="scum-programmer")
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
    "-c",
    "--calibrate",
    is_flag=True,
    default=False,
    help="Calibrate SCuM after flashing.",
)
@click.argument("firmware", type=click.File(mode="rb"), required=True)
def main(port, baudrate, calibrate, firmware):
    programmer_settings = ScumProgrammerSettings(
        port=port,
        baudrate=baudrate,
        calibrate=calibrate,
        firmware=firmware.name,
    )
    programmer = ScumProgrammer(programmer_settings)
    programmer.run()


if __name__ == "__main__":
    main()
