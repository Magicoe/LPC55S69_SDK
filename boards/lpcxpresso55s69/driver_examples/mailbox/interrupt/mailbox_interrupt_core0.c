/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_mailbox.h"

#include "fsl_common.h"
#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Address of RAM, where the image for core1 should be copied */
#define CORE1_BOOT_ADDRESS (void *)0x20033000

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

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#ifdef CORE1_IMAGE_COPY_TO_RAM
uint32_t get_core1_image_size(void);
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_msg = 1;

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

/* When the core CM0+ writes to CM4 mailbox register it causes call of this irq handler,
   in which CM4 read value sended by CM0+, increments it and send it again to CM0+ */
void MAILBOX_IRQHandler()
{
#if (defined(LPC55S69_cm33_core0_SERIES) || defined(LPC55S69_cm33_core1_SERIES))
    g_msg = MAILBOX_GetValue(MAILBOX, kMAILBOX_CM33_Core0);
    MAILBOX_ClearValueBits(MAILBOX, kMAILBOX_CM33_Core0, 0xffffffff);
    PRINTF("Read value from CM33 Core0 mailbox register: %d\r\n", g_msg);
    g_msg++;
    PRINTF("Write to CM33 Core1 mailbox register: %d\r\n", g_msg);
    MAILBOX_SetValue(MAILBOX, kMAILBOX_CM33_Core1, g_msg);
#else
    g_msg = MAILBOX_GetValue(MAILBOX, kMAILBOX_CM4);
    MAILBOX_ClearValueBits(MAILBOX, kMAILBOX_CM4, 0xffffffff);
    PRINTF("Read value from CM4 mailbox register: %d\r\n", g_msg);
    g_msg++;
    PRINTF("Write to CM0+ mailbox register: %d\r\n", g_msg);
    MAILBOX_SetValue(MAILBOX, kMAILBOX_CM0Plus, g_msg);
#endif
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware.*/
    /* attach main clock divide to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitPins_Core0();
    BOARD_BootClockFROHF96M();
    BOARD_InitDebugConsole();

    PRINTF("Mailbox interrupt example\r\n");

    /* Init Mailbox */
    MAILBOX_Init(MAILBOX);

    /* Enable mailbox interrupt */
    NVIC_EnableIRQ(MAILBOX_IRQn);

#ifdef CORE1_IMAGE_COPY_TO_RAM
    /* Calculate size of the image */
    uint32_t core1_image_size;
    core1_image_size = get_core1_image_size();
    PRINTF("Copy CORE1 image to address: 0x%x, size: %d\r\n", CORE1_BOOT_ADDRESS, core1_image_size);

    /* Copy application from FLASH to RAM */
    memcpy(CORE1_BOOT_ADDRESS, (void *)CORE1_IMAGE_START, core1_image_size);
#endif

#if (defined(LPC55S69_cm33_core0_SERIES) || defined(LPC55S69_cm33_core1_SERIES))
    /* Boot source for Core 1 from flash */
    SYSCON->CPUCFG |= SYSCON_CPUCFG_CPU1ENABLE_MASK;
    SYSCON->CPBOOT = SYSCON_CPBOOT_CPBOOT(CORE1_BOOT_ADDRESS);

    int32_t temp = SYSCON->CPUCTRL;
    temp |= 0xc0c48000;
    SYSCON->CPUCTRL = temp | SYSCON_CPUCTRL_CPU1RSTEN_MASK | SYSCON_CPUCTRL_CPU1CLKEN_MASK;
    SYSCON->CPUCTRL = (temp | SYSCON_CPUCTRL_CPU1CLKEN_MASK) & (~SYSCON_CPUCTRL_CPU1RSTEN_MASK);
#else
    /* Boot source for Core 1 from flash */
    SYSCON->CPBOOT = SYSCON_CPBOOT_BOOTADDR(*(uint32_t *)((uint8_t *)CORE1_BOOT_ADDRESS + 0x4));
    SYSCON->CPSTACK = SYSCON_CPSTACK_STACKADDR(*(uint32_t *)CORE1_BOOT_ADDRESS);

    int32_t temp = SYSCON->CPUCTRL;
    temp |= 0xc0c48000;
    SYSCON->CPUCTRL = (temp | SYSCON_CPUCTRL_CM0RSTEN_MASK);
    SYSCON->CPUCTRL = (temp);
#endif

#if (defined(LPC55S69_cm33_core0_SERIES) || defined(LPC55S69_cm33_core1_SERIES))
    PRINTF("Write to CM33 Core1 mailbox register: %d\r\n", g_msg);
    /* Write g_msg to CM33 Core1 mailbox register - it causes interrupt on CM33 Core1 */
    MAILBOX_SetValue(MAILBOX, kMAILBOX_CM33_Core1, g_msg);
#else
    PRINTF("Write to CM0+ mailbox register: %d\r\n", g_msg);
    /* Write g_msg to CM0+ mailbox register - it causes interrupt on CM0+ core */
    MAILBOX_SetValue(MAILBOX, kMAILBOX_CM0Plus, g_msg);
#endif

    while (1)
    {
        __WFI();
    }
}
