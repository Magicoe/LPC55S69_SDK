/*
 * Copyright (c) 2018, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_plu.h"

#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief API to configure a LUT for input, truth table and output
 * User can use the code that generate from PLU tool to replace current lut configurations in this function.
 */
static void PLU_SetLutAllInputOutputTruthTable(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void PLU_SetLutAllInputOutputTruthTable(void)
{
    /* Select plu_input(3) to be connected to LUT0 input 0 */
    PLU_SetLutInputSource(PLU, kPLU_LUT_0, kPLU_LUT_IN_0, kPLU_LUT_IN_SRC_PLU_IN_3);
    PLU_SetLutTruthTable(PLU, kPLU_LUT_0, 0x00000001);
    /* Select LUT_out(0) to be connected to PLU output 0 */
    PLU_SetOutputSource(PLU, kPLU_OUTPUT_0, kPLU_OUT_SRC_LUT_0);

    /* Select LUT_out(0) to be connected to LUT1 input 0 */
    PLU_SetLutInputSource(PLU, kPLU_LUT_1, kPLU_LUT_IN_0, kPLU_LUT_IN_SRC_LUT_OUT_0);
    /* Select plu_input(4) to be connected to LUT1 input 1 */
    PLU_SetLutInputSource(PLU, kPLU_LUT_1, kPLU_LUT_IN_1, kPLU_LUT_IN_SRC_PLU_IN_4);
    PLU_SetLutTruthTable(PLU, kPLU_LUT_1, 0x00000008);
    /* Select LUT_out(1) to be connected to PLU output 1 */
    PLU_SetOutputSource(PLU, kPLU_OUTPUT_1, kPLU_OUT_SRC_LUT_1);

    /* Select plu_input(5) to be connected to LUT2 input 0 */
    PLU_SetLutInputSource(PLU, kPLU_LUT_2, kPLU_LUT_IN_0, kPLU_LUT_IN_SRC_PLU_IN_5);
    PLU_SetLutTruthTable(PLU, kPLU_LUT_2, 0x00000001);
    /*
     * Select Flip-Flops state(2) to be connected to PLU output 2
     * Note: An external clock must be applied to the PLU_CLKIN input when using FFs.
     */
    PLU_SetOutputSource(PLU, kPLU_OUTPUT_2, kPLU_OUT_SRC_FLIPFLOP_2);
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Board pin, clock */
    BOARD_InitPins();
    BOARD_BootClockFROHF96M();

    /* Init plu module */
    PLU_Init(PLU);

    /* Configure input, output, truthtable one time through the API */
    PLU_SetLutAllInputOutputTruthTable();

    /* Once the PLU module is configured, the PLU bus clock can be shut-off to conserve power */
    PLU_Deinit(PLU);

    while (1)
    {
    }
}
