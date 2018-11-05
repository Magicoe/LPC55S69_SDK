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

#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include "stdint.h"
#include "arm_cmse.h"
#include "veneer_table.h"
#include "fsl_debug_console.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_STRING_LENGTH         0x400
#if defined(__IAR_SYSTEMS_ICC__)
typedef __cmse_nonsecure_call int (*callbackptr_NS)(char const * s1, char const * s2);
#else
typedef int (*callbackptr_NS)(char const * s1, char const * s2) __attribute__((cmse_nonsecure_call));
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* strnlen function implementation for arm compiler */
#if defined(__arm__)
size_t strnlen(const char* s, size_t maxLength)
{
    size_t length = 0;
    while ((length <= maxLength) && (*s))
    {
      s++;
      length++;
    }
    return length;
}
#endif

#if defined(__IAR_SYSTEMS_ICC__)
__cmse_nonsecure_entry void DbgConsole_Printf_NSE(char const *s)
#else
__attribute__((cmse_nonsecure_entry)) void DbgConsole_Printf_NSE(char const *s)
#endif
{
    size_t  string_length;
    /* Access to non-secure memory from secure world has to be properly validated */
    /* Check whether string is properly terminated */
    string_length = strnlen(s, MAX_STRING_LENGTH);
    if ((string_length == MAX_STRING_LENGTH) && (s[string_length] != '\0'))
    {
        PRINTF("String too long or invalid string termination!\r\n");
         abort();
    }

    /* Check whether string is located in non-secure memory */
    if (cmse_check_address_range((void *)s, string_length, CMSE_NONSECURE | CMSE_MPU_READ) == NULL)
    {
        PRINTF("String is not located in normal world!\r\n");
        abort();
    }
    PRINTF(s);
}

/* Non-secure callable (entry) function, calling a non-secure callback function */
#if defined(__IAR_SYSTEMS_ICC__)
__cmse_nonsecure_entry uint32_t StringCompare_NSE(volatile callbackptr callback, char const *s1, char const *s2)
#else
__attribute__((cmse_nonsecure_entry)) uint32_t StringCompare_NSE(volatile callbackptr callback, char const *s1, char const *s2)
#endif
{
    callbackptr_NS callback_NS;
    size_t  string_length;
    int result;
 
    /* Input parameters check */
    /* Check whether function pointer is located in non-secure memory */
    callback_NS = (callbackptr_NS)cmse_nsfptr_create(callback);
    if (cmse_check_pointed_object((int *)callback_NS, CMSE_NONSECURE) == NULL)
    {
        PRINTF("The callback is not located in normal world!\r\n");
        abort();
    }
    /* Check whether string is properly terminated */
    string_length = strnlen(s1, MAX_STRING_LENGTH);
    if ((string_length == MAX_STRING_LENGTH) && (s1[string_length] != '\0'))
    {
        PRINTF("First string too long or invalid string termination!\r\n");
         abort();
    }
    /* Check whether string is properly terminated */
    string_length = strnlen(s2, MAX_STRING_LENGTH);
    if ((string_length == MAX_STRING_LENGTH) && (s2[string_length] != '\0'))
    {
        PRINTF("Second string too long or invalid string termination!\r\n");
         abort();
    }
    PRINTF("Comparing two string as a callback to normal world\r\n");
    PRINTF("String 1: ");
    PRINTF(s1);
    PRINTF("String 2: ");
    PRINTF(s2);
    result = callback_NS(s1, s2);
    return result;
}

