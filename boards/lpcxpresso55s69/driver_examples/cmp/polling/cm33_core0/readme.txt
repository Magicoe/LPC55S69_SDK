Overview
========

The CMP interrupt Example shows how to use interrupt with CMP driver.

In this example, user should use one external input mux,supply the pmux voltage.

When running the project, change the input voltage of p side input source, then the comparator's output would change
between logic one and zero. The CPU check the output of comparator all time. When the output is high,the blue LED will
turn on,otherwise the blue Led will turn off.
Hardware requirements
=====================
- Mini/micro USB cable
- LPCxpresso55s69 board
- Personal Computer

Board settings
==============
(pmux)pio0_18 connect to voltage source.

Prepare the Demo
================
1.  Connect a micro USB cable between the PC host and the OpenSDA USB port on the board.
2.  Open a serial terminal on PC for OpenSDA serial device with these settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Reset the SoC and run the project.

Running the demo
================
When the demo runs successfully, the log would be seen on the OpenSDA terminal like:

CMP polling Example.
BLUE LED turn on.(pmux >= 3.3V).
BLUE LED turn off.(pmux < 3.3V).


Toolchain supported
===================
- IAR embedded Workbench 
- Keil MDK 
- GCC ARM Embedded 
- MCUXpresso

