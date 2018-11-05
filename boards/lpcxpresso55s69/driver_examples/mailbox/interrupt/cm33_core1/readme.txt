Overview
========
The mailbox_interrupt example shows how to use mailbox to exchange message.

In this example:
The core 0(CM4) writes value to mailbox for Core 1(CM0+), it causes mailbox interrupt
on CM0+ side. CM0+ reads value from mailbox increments and writes it to mailbox register
for CM4, it causes mailbox interrupt on CM4 side. CM4 reads value from mailbox increments
and writes it to mailbox register for CM0 again.
Hardware requirements
=====================
- Mini/micro USB cable
- LPCXpresso55s69 board
- Personal Computer

Board settings
==============
No special settings are required.

Prepare the Demo
================
1.  Connect a micro USB cable between the PC host and the CMSIS DAP USB port (P6) on the board
2.  Open a serial terminal with the following settings (See Appendix A in Getting started guide for description how to determine serial port number):
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

Running the demo
================
The log below shows example output of the Mailbox interrupt example in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mailbox interrupt example
Copy CORE1 image to address: 0x20033000, size: 1388
Write to CM33 Core1 mailbox register: 1
Read value from CM33 Core0 mailbox register: 2
Write to CM33 Core1 mailbox register: 3
Read value from CM33 Core0 mailbox register: 4
Write to CM33 Core1 mailbox register: 5
Read value from CM33 Core0 mailbox register: 6
Write to CM33 Core1 mailbox register: 7
.
.
.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Toolchain supported
===================
- IAR embedded Workbench 
- MCUXpresso
- GCC ARM Embedded 
- Keil MDK 

