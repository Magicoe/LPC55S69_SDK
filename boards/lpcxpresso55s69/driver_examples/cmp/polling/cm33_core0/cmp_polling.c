/*
 * Copyright 2018 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_cmp.h"

#include "pin_mux.h"
#include <stdbool.h>
#include "fsl_gpio.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_USART USART0
#define DEMO_USART_CLK_SRC kCLOCK_Fro32M


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void LED_ON(void);
void LED_OFF(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void LED_ON(void)
{
    GPIO_PinWrite(GPIO, BOARD_LED_BLUE_GPIO_PORT, BOARD_LED_BLUE_GPIO_PIN, 0);
}

void LED_OFF(void)
{
    GPIO_PinWrite(GPIO, BOARD_LED_BLUE_GPIO_PORT, BOARD_LED_BLUE_GPIO_PIN, 1);
}
/*!
 * @brief Main function
 */
int main(void)
{
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
    config.nmuxInput = kCMP_NInputVREF;
    CMP_Init(&config);

    /*select VDDA as VREF*/
    CMP_VREFSelect(KCMP_VREFSelectVDDA);
    CMP_SetRefStep(31U);
    CMP_ClearStatus();
    PRINTF("\r\nCMP polling Example.\r\n");

    while (1)
    {
        if (CMP_GetOutput())
        {
            LED_ON();
        }
        else
        {
            LED_OFF();
        }
    }
}
