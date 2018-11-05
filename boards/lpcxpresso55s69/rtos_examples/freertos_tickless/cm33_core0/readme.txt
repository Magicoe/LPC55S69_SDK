Overview
=========
This document explains the freertos_tickless example. It shows the CPU enter at sleep mode and then
it is waked up by expired time delay that using RTC module or occur any a hardware interrupt.

Prepare the Demo
================
1.  Connect a micro USB cable between the PC host and the CMSIS DAP USB port (J7) on the board
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
After the board is flashed the Tera Term will start periodically printing number tick count when the CPU
is running.

When the CPU is running and get tick count number, press button on board that suggest message display on
console screen ("eg:Press SW3 <or SW1,SW2> to wake up the CPU") to wake up CPU then Tera Term
will printing “CPU waked up by EXT interrupt” message.

Example output:

Tickless Demo example
Press SW1 to wake up the CPU
Tick count :
1
5001
CPU waked up by EXT interrupt
10001
CPU waked up by EXT interrupt
15001
CPU waked up by EXT interrupt
CPU waked up by EXT interrupt
20001
25001
30001

Explanation of the example
The example application will get tick count number after every period time delay vTaskDelay(time)
When this function called the CPU enter to sleep mode a period of time that called by vTaskDelay
function.

While the CPU is sleeping, if press SW1 button once or several time on board then CPU waked up
by external interrupt and continuous enter sleep mode after complete interrupt service.
The period of time delay is not change after occur external interrupt.
Hardware requirements
=====================
- Mini/micro USB cable
- LPCXpresso55s69 board
- Personal Computer

Board settings
==============
Connect a USB2COM between the PC host and the board UART pins
boards           -               USB2COM
J14-Pin26                        Tx
J14-Pin28                        Rx
J14-Pin1                         GND


Toolchain supported
===================
- IAR embedded Workbench 
- Keil MDK 
- GCC ARM Embedded 
- MCUXpresso

