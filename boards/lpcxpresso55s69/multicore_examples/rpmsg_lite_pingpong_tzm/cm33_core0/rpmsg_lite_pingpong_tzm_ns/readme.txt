Overview
========
The Multicore RPMsg-Lite pingpong TZM project is a simple demonstration program that uses the
MCUXpresso SDK software and the RPMsg-Lite library and shows how to implement the inter-core
communication between cores in the TrustZone-M (TZM) environment.
The TZM is configured in the BOARD_InitTrustZone() function. The secondary core is put into
the secure domain as well as the shared memory for RPMsg messages exchange. The primary core 
boots in the secure domain, it releases the secondary core from the reset and then the inter-core 
communication is established in the secure domain. Once the RPMsg is initialized
and the secure endpoint is created the message exchange starts, incrementing a virtual counter that is part
of the message payload. The message pingpong in the secure domain finishes when the counter reaches 
the value of 50. Then the secure domain is kept running and the non-secure portion of the application
is started. The non-secure RPMsg-Lite endpoint is created in the non-secure domain and the new message 
pingpong is triggered, exchanging data between the secondary core secure endpoint and the primary core 
non-secure endpoint. Veneer functions defined in the secure project are called from the non-secure
domain during this process. This message pingpong finishes when the counter reaches the value of 1050. 
Debug prints from the non-secure portion of the application are routed into the secure domain using
the dedicated veneer function.

Shared memory usage
This multicore example uses the shared memory for data exchange. The shared memory region is
defined and the size can be adjusted in the linker file. The shared memory region start address
and the size have to be defined in linker file for each core equally. The shared memory start
address is then exported from the linker to the application.
The shared memory assignment to the secure domain is done in the BOARD_InitTrustZone() function.
Hardware requirements
=====================
- Mini/micro USB cable
- LPCXpresso55s69 board
- Personal Computer

Board settings
==============
The Multicore RPMsg-Lite pingpong TZM project does not call for any special hardware configurations.
Although not required, the recommendation is to leave the development board jumper settings and 
configurations in default state when running this demo.


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

For detailed instructions, see the appropriate board User's Guide.

Running the demo
================
The log below shows the output of the RPMsg-Lite pingpong TZM demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Copy CORE1 image to address: 0x30033000, size: 9344

RPMsg demo starts

Data exchange in secure domain
Primary core received a msg
Message: Size=4, DATA = 1
Primary core received a msg
Message: Size=4, DATA = 3
Primary core received a msg
Message: Size=4, DATA = 5
.
.
.
Primary core received a msg
Message: Size=4, DATA = 51

Entering normal world now.

Non-secure portion of the application started!

Data exchange in non-secure domain
Primary core received a msg
Message: DATA = 1001
Primary core received a msg
Message: DATA = 1003
Primary core received a msg
Message: DATA = 1005
.
.
.
Primary core received a msg
Message: DATA = 1049

RPMsg demo ends
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Toolchain supported
===================
- GCC ARM Embedded 
- MCUXpresso
- IAR embedded Workbench 
- Keil MDK 

