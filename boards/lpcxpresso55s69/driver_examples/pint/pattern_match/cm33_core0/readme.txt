Overview
========
This example shows how to use SDK drivers to use the Pin interrupt & pattern match peripheral.
Hardware requirements
=====================
- Micro USB cable
- LPCXpresso55s69 board
- Personal Computer

Board settings
==============
No special settings are required.

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

Running the demo
================
1.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

The following lines are printed to the serial terminal when the demo program is executed.

PINT Pattern Match example

PINT Pattern match events are configured

Press corresponding switches to generate events

2. This example configures "Pin Interrupt 0" to be invoked when SW1 switch is pressed by the user. 
   Bit slice 0 is configured as an endpoint in sticky falling edge mode. The interrupt callback prints 
   "PINT Pin Interrupt 0 event detected. PatternMatch status =        1". 
   
   "Pin Interrupt 2" is configured to be invoked when rising edge on SW1, SW2 is detected. The 
   interrupt callback prints "PINT Pin Interrupt 2 event detected. PatternMatch status =     100". Bit slices
   1 configured to detect sticky rising edge. Bit slice 2 is configured as an endpoint.

   "Pin Interrupt 3" is configured to be invoked when rising edge on SW1, SW2 and SW3 is detected. The 
   interrupt callback prints "PINT Pin Interrupt 3 event detected. PatternMatch status =     1000". Bit slices
   1, 2 and 3 configured to detect sticky rising edge. Bit slice 3 is configured as an endpoint.
   
   "Pin Interrupt 6" is configured to be invoked when rising edge on SW1, low level on SW2 and SW3 is detected. 
   The interrupt callback prints "PINT Pin Interrupt 6 event detected. PatternMatch status =  1000000". Bit slice
   4 configured to detect sticky rising edge. Bit slices 5 and 6 are configured to detect low level. Bit slice 6 
   is configured as an endpoint.

Toolchain supported
===================
- IAR embedded Workbench 
- Keil MDK 
- GCC ARM Embedded 
- MCUXpresso

