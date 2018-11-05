/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"

#if defined(FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER) && FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER
#include "fsl_syscon.h"
#else
#include "fsl_inputmux.h"
#endif /* FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER */
#include "fsl_pint.h"

#include "fsl_common.h"
#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_PINT_PIN_INT0_SRC kINPUTMUX_GpioPort0Pin5ToPintsel
#define DEMO_PINT_PIN_INT1_SRC kINPUTMUX_GpioPort1Pin18ToPintsel
#define DEMO_PINT_PIN_INT2_SRC kINPUTMUX_GpioPort1Pin9ToPintsel

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
* @brief Call back for PINT Pin interrupt 0-7.
*/
void pint_intr_callback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
    PRINTF("\f\r\nPINT Pin Interrupt %d event detected. PatternMatch status = %8b\r\n", pintr, pmatch_status);
}

/*!
* @brief Main function
*/
int main(void)
{
    pint_pmatch_cfg_t pmcfg;

    /* Board pin, clock, debug console init */
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM0);

    BOARD_InitPins();
    BOARD_BootClockFROHF96M();
    BOARD_InitDebugConsole();

    /* Clear screen*/
    PRINTF("%c[2J", 27);
    /* Set cursor location at [0,0] */
    PRINTF("%c[0;0H", 27);
    PRINTF("\f\r\nPINT Pattern Match example\r\n");

#if defined(FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER) && FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER
    /* Connect trigger sources to PINT */
    SYSCON_AttachSignal(SYSCON, kPINT_PinInt0, DEMO_PINT_PIN_INT0_SRC);
    SYSCON_AttachSignal(SYSCON, kPINT_PinInt1, DEMO_PINT_PIN_INT1_SRC);
#else
    /* Connect trigger sources to PINT */
    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, DEMO_PINT_PIN_INT0_SRC);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt1, DEMO_PINT_PIN_INT1_SRC);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt2, DEMO_PINT_PIN_INT2_SRC);

    /* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX);
#endif /* FSL_FEATURE_SYSCON_HAS_PINT_SEL_REGISTER */

    /* Initialize PINT */
    PINT_Init(PINT);

    /* Setup Pattern Match Bit Slice 0 */
    pmcfg.bs_src = kPINT_PatternMatchInp0Src;
    pmcfg.bs_cfg = kPINT_PatternMatchStickyFall;
    pmcfg.callback = pint_intr_callback;
    pmcfg.end_point = true;
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice0, &pmcfg);

    /* Enable callbacks for PINT0 by Index */
    PINT_EnableCallbackByIndex(PINT,kPINT_PinInt0);

#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 1U)
    /* Setup Pattern Match Bit Slice 1 */
    pmcfg.bs_src = kPINT_PatternMatchInp0Src;
    pmcfg.bs_cfg = kPINT_PatternMatchStickyRise;
    pmcfg.callback = pint_intr_callback;
    pmcfg.end_point = false;
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice1, &pmcfg);
#endif

#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 2U)
    /* Setup Pattern Match Bit Slice 2 for falling edge detection */
    pmcfg.bs_src = kPINT_PatternMatchInp1Src;
    pmcfg.bs_cfg = kPINT_PatternMatchStickyRise;
    pmcfg.callback = pint_intr_callback;
    pmcfg.end_point = true;
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice2, &pmcfg);
    
    /* Enable callbacks for PINT2 by Index */
    PINT_EnableCallbackByIndex(PINT,kPINT_PinInt2); 
#endif

#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 3U)
    /* Setup Pattern Match Bit Slice 3 for rising edge detection */
    pmcfg.bs_src = kPINT_PatternMatchInp2Src;
    pmcfg.bs_cfg = kPINT_PatternMatchStickyRise;
    pmcfg.callback = pint_intr_callback;
    pmcfg.end_point = true;
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice3, &pmcfg);

    /* Enable callbacks for PINT3 by Index */ 
    PINT_EnableCallbackByIndex(PINT,kPINT_PinInt3); 
#endif

#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 4U)
    /* Setup Pattern Match Bit Slice 4  for rising edge detection */
    pmcfg.bs_src = kPINT_PatternMatchInp0Src;
    pmcfg.bs_cfg = kPINT_PatternMatchStickyRise;
    pmcfg.callback = pint_intr_callback;
    pmcfg.end_point = false;
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice4, &pmcfg);
#endif

#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 5U)
    /* Setup Pattern Match Bit Slice 5 for low level detection */
    pmcfg.bs_src = kPINT_PatternMatchInp1Src;
    pmcfg.bs_cfg = kPINT_PatternMatchLow;
    pmcfg.callback = pint_intr_callback;
    pmcfg.end_point = false;
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice5, &pmcfg);
#endif

#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 6U)
    /* Setup Pattern Match Bit Slice 6 for low level detection */
    pmcfg.bs_src = kPINT_PatternMatchInp2Src;
    pmcfg.bs_cfg = kPINT_PatternMatchLow;
    pmcfg.callback = pint_intr_callback;
    pmcfg.end_point = true;
    PINT_PatternMatchConfig(PINT, kPINT_PatternMatchBSlice6, &pmcfg);

    /* Enable callbacks for PINT6 by Index */ 
    PINT_EnableCallbackByIndex(PINT,kPINT_PinInt6);
#endif

    /* Enable PatternMatch */
    PINT_PatternMatchEnable(PINT);

    PRINTF("\r\nPINT Pattern match events are configured\r\n");
    PRINTF("\r\nPress corresponding switches to generate events\r\n");
    while (1)
    {
        __WFI();
    }
}
