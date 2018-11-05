Overview
========

The CMP interrupt Example shows how to use interrupt with CMP driver.

In this example, user should use two external input mux,supply the pmux and nmux diffrent voltage.

When running the project, change the input voltage of two side input source, then the comparator's output would change
between logic one and zero. The comparator's output would generate the highlevel and lowlevel events with their interrupts enabled. When any CMP 
interrupt happens, It prints the "p is higher than n" string or "p is lower than n" to the terminal.
Hardware requirements
=====================
- Mini/micro USB cable
- LPCxpresso55s69 board
- Personal Computer

Board settings
==============
(pmux)pin0_18 connect to  voltage source1.
(nmux)pin1_14 connect to  voltage source2.

Prepare the Demo
================
1.  Connect a macro USB cable between the PC host and the OpenSDA USB port on the board.
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

CMP interrupt driver example.
Input source P voltage range higher than source N.
Input source P voltage range lower than source N.
CMP interrupt driver example finish.

Toolchain supported
===================
- IAR embedded Workbench 
- Keil MDK 
- GCC ARM Embedded 
- MCUXpresso

