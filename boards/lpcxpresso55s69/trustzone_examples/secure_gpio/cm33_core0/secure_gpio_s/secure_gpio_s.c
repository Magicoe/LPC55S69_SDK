/*
 * The Clear BSD License
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
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

#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include "arm_cmse.h"
#include "veneer_table.h"
#include "tzm_config.h"
#include "fsl_device_registers.h"
#include "board.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define NON_SECURE_START          0x00010000

/* typedef for non-secure callback functions */
#if defined(__IAR_SYSTEMS_ICC__)
__cmse_nonsecure_call typedef void (*funcptr_ns) (void); 
#else
typedef void (*funcptr_ns) (void) __attribute__((__cmse_nonsecure_call));
#endif
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
 * @brief Application-specific implementation of the SystemInitHook() weak function.
 */
void SystemInitHook(void) 
{
/* The TrustZone should be configured as early as possible after RESET.
 * Therefore it is called from SystemInit() during startup. The SystemInitHook() weak function 
 * overloading is used for this purpose.
*/
    BOARD_InitTrustZone();
}

/*!
 * @brief SysTick Handler
 */
void SysTick_Handler(void)
{
        /* Control green LED based on SW1 button press */
        if (GPIO_PinRead(BOARD_SECURE_SW1_GPIO, BOARD_SECURE_SW1_GPIO_PORT, BOARD_SECURE_SW1_GPIO_PIN))
        {
            LED_GREEN_OFF();
        }
        else
        {
            LED_GREEN_ON();
        }
}

/*!
 * @brief Main function
 */
int main(void)
{
    funcptr_ns ResetHandler_ns;

    /* Board pin init */
    BOARD_InitHardware();

    /* Initialize secure GPIO for SW1. */
    CLOCK_EnableClock(kCLOCK_Gpio_Sec);
    RESET_PeripheralReset(kGPIOSEC_RST_SHIFT_RSTn);
    GPIO_PinInit(BOARD_SECURE_SW1_GPIO, BOARD_SECURE_SW1_GPIO_PORT, BOARD_SECURE_SW1_GPIO_PIN, \
                 &(gpio_pin_config_t){kGPIO_DigitalInput, 0});

    /* Initialize GPIO for SW1. */
    GPIO_PortInit(BOARD_SW1_GPIO, BOARD_SW1_GPIO_PORT);
    GPIO_PinInit(BOARD_SW1_GPIO, BOARD_SW1_GPIO_PORT, BOARD_SW1_GPIO_PIN, \
                 &(gpio_pin_config_t){kGPIO_DigitalInput, 0});

    /* Initialize GPIO for LEDs */
    GPIO_PortInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PORT);
    LED_BLUE_INIT(0x1U);
    LED_GREEN_INIT(0x1U);

    /* Set systick reload value to generate 5ms interrupt */
    SysTick_Config(SystemCoreClock / 5000U);

    /* Set non-secure main stack (MSP_NS) */
    __TZ_set_MSP_NS(*((uint32_t *)(NON_SECURE_START)));
 
    /* Set non-secure vector table */
    SCB_NS->VTOR = NON_SECURE_START;
    
    /* Get non-secure reset handler */
    ResetHandler_ns = (funcptr_ns)(*((uint32_t *)((NON_SECURE_START) + 4U)));
     
    /* Call non-secure application - jump to normal world */
    /*  */
    ResetHandler_ns();    
    while (1)
    {
        /* This point should never be reached */
    }
}
