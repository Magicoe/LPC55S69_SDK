/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_mailbox.h"
#include "fsl_debug_console.h"

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

/* Pointer to shared variable by both cores, before changing of this variable the
   cores must first take Mailbox mutex, after changing the shared variable must
   retrun mutex */
volatile uint32_t *g_shared = NULL;

/*******************************************************************************
 * Code
 ******************************************************************************/
void MAILBOX_IRQHandler()
{
#if (defined(LPC55S69_cm33_core0_SERIES) || defined(LPC55S69_cm33_core1_SERIES))
    g_shared = (uint32_t *)MAILBOX_GetValue(MAILBOX, kMAILBOX_CM33_Core1);
    MAILBOX_ClearValueBits(MAILBOX, kMAILBOX_CM33_Core1, 0xffffffff);
#else
    g_shared = (uint32_t *)MAILBOX_GetValue(MAILBOX, kMAILBOX_CM0Plus);
    MAILBOX_ClearValueBits(MAILBOX, kMAILBOX_CM0Plus, 0xffffffff);
#endif
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware */
    BOARD_InitPins_Core1();
    BOARD_InitDebugConsole();

    /* Initialize Mailbox */
    MAILBOX_Init(MAILBOX);

    /* Enable mailbox interrupt */
    NVIC_EnableIRQ(MAILBOX_IRQn);

    while (1)
    {
        /* Get Mailbox mutex */
        while (MAILBOX_GetMutex(MAILBOX) == 0)
            ;

        /* The core1 has mutex, can change shared variable g_shared */
        if (g_shared != NULL)
        {
            (*g_shared)++;
            PRINTF("Core1 has mailbox mutex, update shared variable to: %d\r\n", *g_shared);
        }

        /* Set mutex to allow access other core to shared variable */
        MAILBOX_SetMutex(MAILBOX);
    }
}
