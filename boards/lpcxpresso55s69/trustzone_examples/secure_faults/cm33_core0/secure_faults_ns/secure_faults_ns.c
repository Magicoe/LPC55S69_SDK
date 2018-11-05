/*
 * The Clear BSD License
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"
#include "veneer_table.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef void (*funcptr_t)(char const * s);
#define PRINTF_NSE  DbgConsole_Printf_NSE

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Global variables
 ******************************************************************************/
uint32_t testCaseNumber;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    int result;
    uint32_t *test_ptr;
    volatile uint32_t test_value;
    funcptr_t func_ptr;
    
    BOARD_InitHardware();
	
    testCaseNumber = GetTestCaseNumber_NSE();

    PRINTF_NSE("Welcome in normal world!\r\n");

    /**************************************************************************
    * TEST EXAMPLE 3 - Invalid data access from normal world
    * In this example the pointer is set to address 0x30000000.
    * This address has secure attribute (see SAU settings)
    * If data is read from this address, the secure fault is generated
    * In normal world, the application doesn't have access to secure memory
    **************************************************************************/
    if (testCaseNumber == FAULT_INV_NS_DATA_ACCESS)
    {
        test_ptr = (uint32_t *)(0x30000000U);;
        test_value = *test_ptr;
    }
    /* END OF TEST EXAMPLE 3 */
    
    /**************************************************************************
    * TEST EXAMPLE 4 - Invalid input parameters in entry function
    * In this example the input parameter is set to address 0x30000000.
    * This address has secure attribute (see SAU settings)
    * This secure violation is not detected by secure fault, since the input parameter
    * is used by secure function in secure mode. So this function has access to whole memory
    * However every entry function should check source of all input data in order
    * to avoid potention data leak from secure memory.
    * The correctness of input data cannot be checked automatically.
    * This has to be check by software using TT instruction
    **************************************************************************/
    if (testCaseNumber == FAULT_INV_INPUT_PARAMS)
    {
        PRINTF_NSE((char *)(0x30000000U));
    }
    /* END OF TEST EXAMPLE 4 */
    
    /**************************************************************************
    * TEST EXAMPLE 2 - Invalid entry from normal to secure world
    * In this example function pointer is intentionally increased by 4.
    * By this the SG instruction is skipped, when function is called.
    * This causes ilegal entry point into secure world and secure fault is generated
    * The correct entry point into secure world is ensured by using
    * __cmse_nonsecure_entry keyword attribute for every entry function
    * Then the linker creates veneer table for all entry functions with SG instruction
    **************************************************************************/    
    if (testCaseNumber == FAULT_INV_S_ENTRY)
    {
        func_ptr = (funcptr_t)((uint32_t)&PRINTF_NSE + 4);
        func_ptr("Invalid Test Case\r\n");
    }
    /* END OF TEST EXAMPLE 2 */
    
     /**************************************************************************
    * TEST EXAMPLE 5 - Invalid data access from normal world
    * In this example the pointer is set to address 0x20010000.
    * This address has non-secure attribute in SAU but it has
    * secure attribute in AHB secure controller.
    * If data is read from this address, the data bus error is generated
    * Compare to test example 3, this error is catched by AHB secure controller,
    * not by SAU, because in SAU this address is non-secure so the access from
    * normal world is correct from SAU perspective.
    **************************************************************************/
    if (testCaseNumber == FAULT_INV_NS_DATA2_ACCESS)
    {
        test_ptr = (uint32_t *)(0x20010000U);
        test_value = *test_ptr;
    }
    PRINTF_NSE("This is a text printed from normal world!\r\n");
    
    result = StringCompare_NSE(&strcmp, "Test1\r\n", "Test2\r\n");
    if (result == 0)
    {
        PRINTF_NSE("Both strings are equal!\r\n");
    }
    else
    {
        PRINTF_NSE("Both strings are not equal!\r\n");
    }
    while (1)
    {
    }
}
