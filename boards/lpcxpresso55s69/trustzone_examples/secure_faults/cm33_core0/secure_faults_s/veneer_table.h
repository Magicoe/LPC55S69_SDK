/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
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
#ifndef _VENEER_TABLE_
#define _VENEER_TABLE_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef  int (*callbackptr)(char const * s1, char const * s2);

/* NOTE: These defines are not related to veneer table. But since they are needed
 *       in both secure and non-secure project, they are placed here */
#define FAULT_NONE                  0
#define FAULT_INV_S_TO_NS_TRANS     1
#define FAULT_INV_S_ENTRY           2
#define FAULT_INV_NS_DATA_ACCESS    3
#define FAULT_INV_INPUT_PARAMS      4
#define FAULT_INV_NS_DATA2_ACCESS   5


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Entry function for debug PRINTF (DbgConsole_Printf)
 *
 * This function provides interface between secure and normal worlds
 * This function is called from normal world only
 *
 * @param s     String to be printed
 *
*/
void DbgConsole_Printf_NSE(char const * s);

/*!
 * @brief Entry function for two string comparison
 *
 * This function compares two strings by using non-secure callback
 * This function is called from normal world only
 *
 * @param callback     pointer to strcmp() function
 * @param s1           String to be compared
 * @param s2           String to be compared
 * @return             result of strcmp function
 *                     >0 for s1 > s2
 *                     =0 for s1 = s2
 *                     <0 for s1 < s2
*/
uint32_t StringCompare_NSE(volatile callbackptr callback, char const *s1, char const *s2);


/*!
 * @brief Entry function for GetTestCaseNumber(void)
 *
 * This function retuns number of actual fault testcase
 * This function is called from normal world only
 *
 * @return             number of actual fault testcase
*/
uint32_t GetTestCaseNumber_NSE(void);
#endif /* _VENEER_TABLE_ */
