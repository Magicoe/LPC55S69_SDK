/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "board.h"
#include "mcmgr.h"

#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Address of RAM, where the image for core1 should be copied */
#define CORE1_BOOT_ADDRESS (void *)0x20033000

#define BUTTON_1_INIT() GPIO_PinInit(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PORT, BOARD_SW3_GPIO_PIN, &sw_config)
#define IS_BUTTON_1_PRESSED() !GPIO_PinRead(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PORT, BOARD_SW3_GPIO_PIN)
#define BUTTON_1_NAME BOARD_SW3_NAME

#define BUTTON_2_INIT() GPIO_PinInit(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PORT, BOARD_SW2_GPIO_PIN, &sw_config)
#define IS_BUTTON_2_PRESSED() !GPIO_PinRead(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PORT, BOARD_SW2_GPIO_PIN)
#define BUTTON_2_NAME BOARD_SW2_NAME

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
extern uint32_t Image$$CORE1_REGION$$Base;
extern uint32_t Image$$CORE1_REGION$$Length;
#define CORE1_IMAGE_START &Image$$CORE1_REGION$$Base
#elif defined(__ICCARM__)
extern unsigned char core1_image_start[];
#define CORE1_IMAGE_START core1_image_start
#elif defined(__GNUC__)
extern const char m0_image_start[];
extern const char *m0_image_end;
extern int m0_image_size;
#define CORE1_IMAGE_START ((void *)m0_image_start)
#define CORE1_IMAGE_SIZE ((void *)m0_image_size)
#endif
#define APP_READY_EVENT_DATA (1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#ifdef CORE1_IMAGE_COPY_TO_RAM
uint32_t get_core1_image_size(void);
#endif
void delay(void);
void MCMGR_RemoteCoreUpEventHandler(uint16_t remoteData, void *context);
void MCMGR_RemoteCoreDownEventHandler(uint16_t remoteData, void *context);
void MCMGR_RemoteExceptionEventHandler(uint16_t remoteData, void *context);

/*******************************************************************************
 * Code
 ******************************************************************************/

#ifdef CORE1_IMAGE_COPY_TO_RAM
uint32_t get_core1_image_size(void)
{
    uint32_t core1_image_size;
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
    core1_image_size = (uint32_t)&Image$$CORE1_REGION$$Length;
#elif defined(__ICCARM__)
#pragma section = "__sec_core"
    core1_image_size = (uint32_t)__section_end("__sec_core") - (uint32_t)&core1_image_start;
#elif defined(__GNUC__)
    core1_image_size = (uint32_t)m0_image_size;
#endif
    return core1_image_size;
}
#endif
volatile uint16_t RemoteReadyEventData = 0;

static void RemoteReadyEventHandler(uint16_t eventData, void *context)
{
    RemoteReadyEventData = eventData;
}

/*!
 * @brief Application-specific implementation of the SystemInitHook() weak function.
 */
void SystemInitHook(void)
{
    /* Initialize MCMGR - low level multicore management library. Call this
       function as close to the reset entry as possible to allow CoreUp event
       triggering. The SystemInitHook() weak function overloading is used in this
       application. */
    MCMGR_EarlyInit();
}

/*!
 * @brief Main function
 */
int main(void)
{
    volatile int resetDone = 0;
    volatile int exceptionNumber = 0;
    volatile int startupDone = 0;
    /* Define the init structure for the switches*/
    gpio_pin_config_t sw_config = {kGPIO_DigitalInput, 0};

    /* Initialize MCMGR, install generic event handlers */
    MCMGR_Init();

    /* Init board hardware.*/
    CLOCK_EnableClock(kCLOCK_Gpio1); /* Enable the clock for GPIO0. */

    /* attach main clock divide to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitPins_Core0();
    BOARD_BootClockFROHF96M();
    BOARD_InitDebugConsole();

    /* Configure BUTTONs */
    BUTTON_1_INIT();
    BUTTON_2_INIT();

    /* Print the initial banner from Primary core */
    PRINTF("\r\nHello World from the Primary Core!\r\n\n");

#ifdef CORE1_IMAGE_COPY_TO_RAM
    /* Calculate size of the image  - not required on MCUXpresso IDE. MCUXpresso copies the secondary core
       image to the target memory during startup automatically */
    uint32_t core1_image_size;
    core1_image_size = get_core1_image_size();
    PRINTF("Copy Secondary core image to address: 0x%x, size: %d\r\n", CORE1_BOOT_ADDRESS, core1_image_size);

    /* Copy Secondary core application from FLASH to the target memory. */
    memcpy(CORE1_BOOT_ADDRESS, (void *)CORE1_IMAGE_START, core1_image_size);
#endif

    /* Install remote core up event handler */
    MCMGR_RegisterEvent(kMCMGR_RemoteCoreUpEvent, MCMGR_RemoteCoreUpEventHandler, (void *)&startupDone);

    /* Install remote core down event handler */
    MCMGR_RegisterEvent(kMCMGR_RemoteCoreDownEvent, MCMGR_RemoteCoreDownEventHandler, (void *)&resetDone);

    /* Install remote exception event handler */
    MCMGR_RegisterEvent(kMCMGR_RemoteExceptionEvent, MCMGR_RemoteExceptionEventHandler, (void *)&exceptionNumber);

    /* Register the application event before starting the secondary core */
    MCMGR_RegisterEvent(kMCMGR_RemoteApplicationEvent, RemoteReadyEventHandler, NULL);

    /* Boot Secondary core application */
    PRINTF("Starting Secondary core.\r\n");
    MCMGR_StartCore(kMCMGR_Core1, CORE1_BOOT_ADDRESS, 5, kMCMGR_Start_Synchronous);

    /* Wait until the secondary core application signals that it has been started. */
    while (APP_READY_EVENT_DATA != RemoteReadyEventData)
    {
    };

    PRINTF("The secondary core application has been started.\r\n\r\n");
    PRINTF("Press the %s button to Stop Secondary core.\r\n", BUTTON_1_NAME);
    PRINTF("Press the %s button to Start Secondary core again.\r\n", BUTTON_2_NAME);
    PRINTF(
        "When no action is taken the secondary core application crashes intentionally after 100 LED toggles (simulated "
        "exception), generating the RemoteExceptionEvent to this core.\r\n");
    PRINTF("Use the Stop and then the Start button to get it running again.\r\n\r\n");

    while (1)
    {
        /* Stop secondary core execution. */
        if (1 == IS_BUTTON_1_PRESSED())
        {
            if (kStatus_MCMGR_Success == MCMGR_StopCore(kMCMGR_Core1))
            {
                PRINTF("Stopped Secondary core.\r\n");
            }
            else
            {
                PRINTF("Secondary core already stopped!\r\n");
            }
            delay();
        }
        /* Start core from reset vector */
        if (1 == IS_BUTTON_2_PRESSED())
        {
            if (kStatus_MCMGR_Success == MCMGR_StartCore(kMCMGR_Core1, CORE1_BOOT_ADDRESS, 5, kMCMGR_Start_Synchronous))
            {
                PRINTF("Started Secondary core.\r\n");
            }
            else
            {
                PRINTF("Secondary core already started!\r\n");
            }
            delay();
        }

        if (resetDone)
        {
            PRINTF("Secondary core HW reset executed.\r\n");
            resetDone = 0;
        }

        if (exceptionNumber)
        {
            PRINTF("Secondary core is in exception number %d.\r\n", exceptionNumber);
            exceptionNumber = 0;
        }

        if (startupDone)
        {
            PRINTF("Secondary core is in startup code.\r\n");
            startupDone = 0;
        }
    }
}

void delay(void)
{
    for (uint32_t i = 0; i < 0x7fffffU; i++)
    {
        __NOP();
    }
}

void MCMGR_RemoteCoreUpEventHandler(uint16_t remoteData, void *context)
{
    int *startupDone = (int *)context;
    *startupDone = 1;
}

void MCMGR_RemoteCoreDownEventHandler(uint16_t remoteData, void *context)
{
    int *resetDone = (int *)context;
    *resetDone = 1;
}

void MCMGR_RemoteExceptionEventHandler(uint16_t remoteData, void *context)
{
    int *exceptionNumber = (int *)context;
    *exceptionNumber = remoteData;
}
