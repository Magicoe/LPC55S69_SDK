Overview
========
The GPIO Example project is a demonstration program that uses the KSDK software to manipulate the general-purpose
outputs. The example use LEDs and buttonsto demonstrates GPIO API for port and pin manipulation (init, set,
clear, and toggle).


Hardware requirements
=====================
- Mini/micro USB cable
- LPCXpresso55s69 board
- Personal Computer

Board settings
==============
This example project does not call for any special hardware configurations.

Prepare the Demo
================
1. Connect a micro USB cable between the PC host and the CMSIS DAP USB port (P6) on the board
2. Open a serial terminal with the following settings (See Appendix A in Getting started guide for description how to determine serial port number):
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3. Download the program to the target board.


Running the demo
================
After the board is flashed the Tera Term will print message on terminal.

Example output:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 GPIO Driver example

 The LED is taking turns to shine.

 Standard port read: 60001824

 Masked port read: 60001824

 Port state: 64001804

 Port state: 64001804

 Port state: 64001804

 Port state: 64001804
 ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- When press sw1 the LED GREEN will turn to shine 

Toolchain supported
===================
- IAR embedded Workbench 
- Keil MDK 
- GCC ARM Embedded 
- MCUXpresso

