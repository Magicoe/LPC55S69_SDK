/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_iap_ffr.h"
#include "fsl_prince.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_IS_PRINCE_ENABLED() ((SYSCON->PERIPHENCFG & SYSCON_PERIPHENCFG_PRINCEEN_MASK) ? 1 : 0)
#define APP_ENCRYPTION_START_ADDR (0x40000)
#define APP_ENCRYPTION_LENGTH (0x2000)

#define APP_PUF_CORE_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
/* Worst-case time in ms to fully discharge PUF SRAM */
#define APP_PUF_DISCHARGE_TIME 400
#define PUF_INTRINSIC_KEY_SIZE 16

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
 * @brief Main function
 */
int main(void)
{
    uint32_t prince_region_base_address = 0;
    uint32_t prince_subregion = 0;
    uint8_t prince_iv_code[52] = {0};
    status_t status;
    flash_config_t flashInstance;
    uint8_t keyCode[PUF_GET_KEY_CODE_SIZE_FOR_KEY_SIZE(PUF_INTRINSIC_KEY_SIZE)];

    /* Init board hardware. */
    /* attach main clock divide to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitPins();
    /* Do not use other clock, otherwise the Flash driver is not working correctly */
    BOARD_BootClockFRO12M();
    BOARD_InitDebugConsole();

    PRINTF("\r\nPRINCE Example.\r\n");

    /* PUF module should be initialized by the ROM code,
       re-initilize here again to be sure it really works */
    PUF_Init(PUF, APP_PUF_DISCHARGE_TIME, APP_PUF_CORE_CLK_FREQ);
    PUF_Start(PUF, (const uint8_t *)(FLASH_KEY_STORE_BASE + 8), PUF_ACTIVATION_CODE_SIZE);

    /* Initialize the Flash and FFR driver that is used for storing
       PRINCE-related configuration into the CFPA and CMPA FFR areas */
    FLASH_Init(&flashInstance);
    FFR_Init(&flashInstance);

    /* Check if the PRINCE engine is present on this device. */
    if (1 != APP_IS_PRINCE_ENABLED())
    {
        PRINTF("\r\nPRINCE engine is not present on this device!\r\n");
        while (1)
        {
        }
    }

    PRINTF(
        "PRINCE engine should be initialized by the bootloader at this point if the activation code is valid and the "
        "PUF Enroll had been performed. Here is the actual PRINCE configuration:\r\n");
    /* Iterate for all PRINCE regions */
    for (int i = kPRINCE_Region0; i <= kPRINCE_Region2; i++)
    {
        status = PRINCE_GetRegionBaseAddress(PRINCE, (prince_region_t)i, &prince_region_base_address);
        PRINTF("PRINCE region %i base address: 0x%x\r\n", i, prince_region_base_address);
        status = PRINCE_GetRegionSREnable(PRINCE, (prince_region_t)i, &prince_subregion);
        PRINTF("PRINCE region %i SR enable mask: 0x%x\r\n", i, prince_subregion);
    }

    /* Allow encryption/decryption for specified flash memory address range.
       Ensure about 800 bytes free space on the stack when calling this routine! */
    status = PRINCE_SetEncryptForAddressRange(kPRINCE_Region1, APP_ENCRYPTION_START_ADDR, APP_ENCRYPTION_LENGTH,
                                              &flashInstance);
    if (status != kStatus_Success)
    {
        PRINTF("\r\nError setting encryption/decryption for (0x%x - 0x%x) address range\r\n", APP_ENCRYPTION_START_ADDR,
               APP_ENCRYPTION_START_ADDR + APP_ENCRYPTION_LENGTH);
    }

    /* It is necessary to generate/load Prince region key at this point. Use the API of the PUF driver to accomplish
       that. See the PUF driver example for details about generating keys for individual Prince regions. */

    /* Generate new intrinsic key with index 0 */
    status = PUF_SetIntrinsicKey(PUF, kPUF_KeyIndex_00, PUF_INTRINSIC_KEY_SIZE, keyCode, sizeof(keyCode));
    if (status != kStatus_Success)
    {
        PRINTF("Error setting intrinsic key!\r\n");
    }

    /* Reconstruct key from keyCode to HW bus for crypto module, kPUF_KeySlot2 corresponds to PRINCE_Region1 */
    status = PUF_GetHwKey(PUF, keyCode, sizeof(keyCode), kPUF_KeySlot2, rand());
    if (status != kStatus_Success)
    {
        PRINTF("Error reconstructing key to HW bus!\r\n");
    }

    /* The PRINCE_Region1 keycode (keyCode) should be written to the Key Store Area (part of the CMPA) now
       to be loaded and reconstructed by the ROM code after each reset sequence. FFR_KeystoreWrite() IAP driver
       function can be used to accomplish that. */

    /* Generate new IV code and store it into the persistent memory.
       Ensure about 800 bytes free space on the stack when calling this routine with the store parameter set to true! */
    status = PRINCE_GenNewIV(kPRINCE_Region1, &prince_iv_code[0], true, &flashInstance);
    if (status != kStatus_Success)
    {
        PRINTF("\r\nGenerating the new IV code failed\r\n");
    }

    /* Load IV code into the PRINCE bus encryption engine. */
    status = PRINCE_LoadIV(kPRINCE_Region1, &prince_iv_code[0]);
    if (status != kStatus_Success)
    {
        PRINTF("\r\nLoading IV code into the PRINCE failed\r\n");
    }

    /* Set PRINCE mask value. */
    PRINCE_SetMask(PRINCE, 0x5555AAAAAAAA5555);

    if (kStatus_Success == status)
    {
        /* Enable data encryption */
        PRINCE_EncryptEnable(PRINCE);

        PRINTF("\r\nPRINCE was succesfully configured for on-the-fly encryption/decryption from 0x%x to 0x%x.\r\n",
               APP_ENCRYPTION_START_ADDR, APP_ENCRYPTION_START_ADDR + APP_ENCRYPTION_LENGTH);

        /* Flash driver can be used here for loading data into the PRINCE - controlled flash area.
           Note, that following conditions must be met (otherwise the error is returned):
           - whole PRINCE subregions (8k block) must be erased at once
           - whole PRINCE subregions (8k block) must be loaded at once (no partial write allowed)
         */

        /* Disable data encryption */
        PRINCE_EncryptDisable(PRINCE);
    }
    PRINTF("Example end.\r\n");

    while (1)
    {
    }
}
