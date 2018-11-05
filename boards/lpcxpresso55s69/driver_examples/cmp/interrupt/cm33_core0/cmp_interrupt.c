/*
 * Copyright 2018 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_cmp.h"
#include "fsl_inputmux.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_USART USART0
#define DEMO_USART_CLK_SRC kCLOCK_Fro32M


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile bool s_cmpStatusChanged = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
void ACMP_CAPT0_IRQHandler(void)
{
    if (CMP_GetStatus())
    {
        CMP_DisableInterrupt();
        s_cmpStatusChanged = true;
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    cmp_interrupt_type_t intType = kCMP_LevelHigh;
    cmp_config_t config;

    /* Initialize hardware */
    /* attach clock for USART(debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    /*enable the power to the analog comparator*/
    PMC->PDRUNCFG0 &= ~PMC_PDRUNCFG0_PDEN_COMP_MASK;

    /*enable gpio0 and gpio1 clock*/
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);

    /*Initialize COMP*/
    config.enHysteris = false;
    config.enLowPower = false;
    config.pmuxInput = kCMP_PInputP0_18;
    config.nmuxInput = kCMP_NInputP1_14;
    CMP_Init(&config);
    PRINTF("CMP interrupt driver example\r\n");

    /* set interrupt configuration*/
    CMP_InterruptSourceSelect(true);
    CMP_InterruptTypeSelect(kCMP_LevelHigh);
    /* enable CMP interrupt */
    CMP_EnableInterrupt();
    CMP_ClearStatus();
    EnableIRQ(ACMP_CAPT0_IRQn);

    while (1)
    {
        if (s_cmpStatusChanged)
        {
            if (CMP_GetOutput())
            {
                PRINTF("Input source P voltage range higher than source N\r\n");
            }
            else
            {
                PRINTF("Input source P voltage range lower than source N\r\n");
                break;
            }
            s_cmpStatusChanged = false;
            intType = (intType == kCMP_LevelHigh ? kCMP_LevelLow : kCMP_LevelHigh);
            CMP_InterruptTypeSelect(intType);
            CMP_ClearStatus();
            CMP_EnableInterrupt();
        }
    }

    PRINTF("CMP interrupt driver example finish\r\n");
}
