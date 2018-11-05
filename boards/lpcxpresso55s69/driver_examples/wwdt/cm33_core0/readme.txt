Overview
========
The WDOG Example project is to demonstrate usage of the KSDK wdog driver.
In this example,quick test is first implemented to test the wdog.
And then after 5 times of refreshing the watchdog, a timeout reset is generated.
Hardware requirements
=====================
- Micro USB cable
- LPCXpresso55s69 board
- Personal Computer

Board settings
==============
No special settings are required.

Please note that, if you run the demo with the J10 unconnected, download maybe failed in the second time, please
connect the J10 while debugging, then the CPU can be booted from ROM, otherwise, with the WWDT running, downloading 
maybe failed.

Prepare the Demo
================
1.  Connect a micro USB cable between the host PC and the LPC-Link USB port (P6) on the target board.
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

Running the demo
================
If the demo run successfully, you will see the red LED blink every two seconds.
The log below shows example output of the WWDT driver demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--- Window mode refresh test start---
Watchdog reset occurred
~~~~~~~~~~~~~~~~~~~~~~

Toolchain supported
===================
- IAR embedded Workbench 
- Keil MDK 
- GCC ARM Embedded 
- MCUXpresso

