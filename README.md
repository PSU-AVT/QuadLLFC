Portland State University Quadcopter LLFC
=========================================

This repository contains firmware for the Portland State University AVT Quadcopter.

Detailed information on the code in this repository and the quadcopter as a whole can be found at http://devel.avt.cecs.pdx.edu.


Building
--------

### Before you begin
You must have a valid arm-none-eabi gnu compiler and toolchain in your path.

If you have LPCXpresso installed you can add the included toolchain to your
path using the following command where the x.x.x_xxx corresponds to what version you have and where you installed it.

> export PATH=$PATH:/usr/local/lpcxpresso_x.x.x_xxx/lpcxpresso/tools/bin:/usr/local/lpcxpresso_x.x.x_xxx/lpcxpresso/bin

### Compilation
To compile run the following command:

> make

### Flashing
If you are using LPCXpresso and an LPC1343 you can flash your device using the
following command:

> make boot

> make flash

The make boot command is required only once after the device is plugged in and
subsequent flashes can be performed by just executing make flash.

### Debug (GDB)
If you are using LPCXpresso and an LPC1343 you can use GDB to debug the
software on the target board by using the following command:

> make boot

> make debug

See the Flashing section for information on make boot.

