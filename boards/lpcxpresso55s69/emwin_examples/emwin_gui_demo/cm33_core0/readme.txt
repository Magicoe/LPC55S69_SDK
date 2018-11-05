Overview
========
The example demonstrates graphical widgets of the emWin library.

Running the demo
================
Known issue: The MCU is not able to generate hardware reset of the LCD due to hardware limitation of the boards interconnection.
Because of this the LCD may not get initialized (stays blank) following a cold start (power on) of the board even if the demo was
correctly programmed to the FLASH memory. In such a case simply press the reset button to restart the demo.
Hardware requirements
=====================
- Micro USB cable
- LPCXpresso55S69 board
- Adafruit TFT LCD shield w/Cap Touch
- Personal Computer

Board settings
==============
Attach the LCD shield to the LPC board.

Prepare the Demo
================
1.  Power the board using a micro USB cable connected to P5 USB port on the board, attach debugger to P7 connector
2.  Build the project.
3.  Download the program to the target board.
4.  Reset the SoC and run the project.


Toolchain supported
===================
- IAR embedded Workbench 
- Keil MDK 
- GCC ARM Embedded 
- MCUXpresso

