/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_mailbox.h"

#include "fsl_common.h"
#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_msg;

/*******************************************************************************
 * Code
 ******************************************************************************/
void MAILBOX_IRQHandler()
{
#if (defined(LPC55S69_cm33_core0_SERIES) || defined(LPC55S69_cm33_core1_SERIES))
    g_msg = MAILBOX_GetValue(MAILBOX, kMAILBOX_CM33_Core1);
    MAILBOX_ClearValueBits(MAILBOX, kMAILBOX_CM33_Core1, 0xffffffff);
    g_msg++;
    MAILBOX_SetValue(MAILBOX, kMAILBOX_CM33_Core0, g_msg);
#else
    g_msg = MAILBOX_GetValue(MAILBOX, kMAILBOX_CM0Plus);
    MAILBOX_ClearValueBits(MAILBOX, kMAILBOX_CM0Plus, 0xffffffff);
    g_msg++;
    MAILBOX_SetValue(MAILBOX, kMAILBOX_CM4, g_msg);
#endif
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware.*/
    BOARD_InitPins_Core1();

    /* Initialize Mailbox */
    MAILBOX_Init(MAILBOX);

    /* Enable mailbox interrupt */
    NVIC_EnableIRQ(MAILBOX_IRQn);

    while (1)
    {
        __WFI();
    }
}
