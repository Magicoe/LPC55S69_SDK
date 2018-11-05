/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_common.h"
#include "fsl_power.h"
#include "fsl_inputmux.h"
#include "fsl_pint.h"
#include "fsl_usart.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define APP_USART_RX_ERROR kUSART_RxError
#define APP_RUNNING_INTERNAL_CLOCK BOARD_BootClockFRO12M()
#define APP_USER_WAKEUP_KEY_NAME "SW3"
#define APP_USER_WAKEUP_KEY_GPIO BOARD_SW3_GPIO
#define APP_USER_WAKEUP_KEY_PORT BOARD_SW3_GPIO_PORT
#define APP_USER_WAKEUP_KEY_PIN BOARD_SW3_GPIO_PIN
#define APP_USER_WAKEUP_KEY_INPUTMUX_SEL kINPUTMUX_GpioPort1Pin9ToPintsel

#define APP_EXCLUDE_FROM_SLEEP (kPDRUNCFG_PD_LDOMEM | kPDRUNCFG_PD_LDODEEPSLEEP)

#define APP_EXCLUDE_FROM_DEEPSLEEP (kPDRUNCFG_PD_DCDC | kPDRUNCFG_PD_FRO192M | kPDRUNCFG_PD_FRO32K)

#define APP_SYSCON_STARTER_MASK SYSCON_STARTERSET_GPIO_INT00_SET_MASK

//#define INPUTMUX INPUTMUX0
const char *gWakeupInfoStr[] = {"Sleep [Press the user key to wakeup]", "Deep Sleep [Press the user key to wakeup]",
#if (defined(FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE) && FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE)
                                "Powerdown [Reset to wakeup]", "Deep Powerdown [Reset to wakeup]"};
#else
                                "Deep Powerdown [Reset to wakeup]", "Deep Powerdown [Reset to wakeup]"};
#endif
uint32_t gCurrentPowerMode;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static uint32_t APP_GetUserSelection(void);
static void APP_InitWakeupPin(void);
static void pint_intr_callback(pint_pin_int_t pintr, uint32_t pmatch_status);

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    CLOCK_EnableClock(kCLOCK_Gpio0); /* Enable the clock for GPIO0. */

    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitPins();
    BOARD_BootClockFROHF96M();
    BOARD_InitDebugConsole();

    /* Running 12 MHz to Core*/
    APP_RUNNING_INTERNAL_CLOCK;

    /* Attach Main Clock as CLKOUT */
    CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);

    APP_InitWakeupPin();

    PRINTF("Power Manager Demo for LPC device.\r\n");
    PRINTF("The \"user key\" is: %s\r\n", APP_USER_WAKEUP_KEY_NAME);

    while (1)
    {
        gCurrentPowerMode = APP_GetUserSelection();
        PRINTF("Entering %s ...\r\n", gWakeupInfoStr[gCurrentPowerMode]);

        /* Enter the low power mode. */
        switch (gCurrentPowerMode)
        {
            case kPmu_Sleep: /* Enter sleep mode. */
                POWER_EnterSleep();
                break;
            case kPmu_Deep_Sleep: /* Enter deep sleep mode. */
                POWER_EnterDeepSleep(APP_EXCLUDE_FROM_DEEPSLEEP);
                break;
#if (defined(FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE) && FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE)
            case kPmu_PowerDown: /* Enter power down mode. */
                PRINTF("Press any key to confirm to enter the power down mode and wakeup the device by reset.\r\n\r\n");
                POWER_EnterPowerDown(APP_EXCLUDE_FROM_DEEPSLEEP);
                break;
#endif
            case kPmu_Deep_PowerDown: /* Enter deep power down mode. */
                PRINTF(
                    "Press any key to confirm to enter the deep power down mode and wakeup the device by "
                    "reset.\r\n\r\n");
                GETCHAR();
                POWER_EnterDeepPowerDown(APP_EXCLUDE_FROM_DEEPSLEEP);
                break;
            default:
                break;
        }

        PRINTF("Wakeup.\r\n");
    }
}

/*
 * Setup a GPIO input pin as wakeup source.
 */
static void APP_InitWakeupPin(void)
{
    gpio_pin_config_t gpioPinConfigStruct;

    /* Set SW pin as GPIO input. */
    gpioPinConfigStruct.pinDirection = kGPIO_DigitalInput;
    GPIO_PinInit(APP_USER_WAKEUP_KEY_GPIO, APP_USER_WAKEUP_KEY_PORT, APP_USER_WAKEUP_KEY_PIN, &gpioPinConfigStruct);

    /* Configure the Input Mux block and connect the trigger source to PinInt channle. */
    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, APP_USER_WAKEUP_KEY_INPUTMUX_SEL); /* Using channel 0. */
    INPUTMUX_Deinit(INPUTMUX); /* Turnoff clock to inputmux to save power. Clock is only needed to make changes */

    /* Enable wakeup for PinInt0. */
    SYSCON->STARTERSET[0] |= APP_SYSCON_STARTER_MASK; /* GPIO pin interrupt 0 wake-up. */

    /* Configure the interrupt for SW pin. */
    PINT_Init(PINT);
    PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableFallEdge, pint_intr_callback);
    PINT_EnableCallback(PINT); /* Enable callbacks for PINT */

    EnableDeepSleepIRQ(PIN_INT0_IRQn);
}

/*
 * Callback function when wakeup key is pressed.
 */
static void pint_intr_callback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
    PRINTF("Pin event occurs\r\n");
}

/*
 * Get user selection from UART.
 */
static uint32_t APP_GetUserSelection(void)
{
    uint32_t ch;

    /* Clear rx overflow error once it happens during low power mode. */
    if (APP_USART_RX_ERROR == (APP_USART_RX_ERROR & USART_GetStatusFlags((USART_Type *)BOARD_DEBUG_UART_BASEADDR)))
    {
        USART_ClearStatusFlags((USART_Type *)BOARD_DEBUG_UART_BASEADDR, APP_USART_RX_ERROR);
    }

    PRINTF(
        "Select an option\r\n"
        "\t1. Sleep mode\r\n"
        "\t2. Deep Sleep mode\r\n"
#if (defined(FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE) && FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE)
        "\t3. power down mode\r\n"
        "\t4. Deep power down mode\r\n");
#else
        "\t3. Deep power down mode\r\n");
#endif
    while (1)
    {
        ch = GETCHAR();
#if (defined(FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE) && FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE)
        if ((ch < '1') || (ch > '4')) /* Only '1', '2', '3' , '4'. */
#else
        if ((ch < '1') || (ch > '3')) /* Only '1', '2', '3'. */
#endif
        {
            continue;
        }
        else
        {
            ch = ch - '1'; /* Only 0, 1, 2 (,3). */
            break;
        }
    }
    switch (ch)
    {
        case 0:
            ch = kPmu_Sleep;
            break;
        case 1:
            ch = kPmu_Deep_Sleep;
            break;
#if (defined(FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE) && FSL_FEATURE_SYSCON_HAS_POWERDOWN_MODE)
        case 2:
            ch = kPmu_PowerDown;
            break;
        case 3:
            ch = kPmu_Deep_PowerDown;
            break;
#else
        case 2:
            ch = kPmu_Deep_PowerDown;
            break;
#endif
        default:
            break;
    }
    return ch;
}
