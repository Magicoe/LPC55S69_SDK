/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stdint.h"
#include <arm_cmse.h>
#include "rpmsg_lite.h"
#include "veneer_table.h"
#include "fsl_debug_console.h"

struct rpmsg_lite_instance *rpmsg_lite_instance_s;
#if defined(RL_USE_STATIC_API) && (RL_USE_STATIC_API == 1)
struct rpmsg_lite_ept_static_context ept_context_s;
#endif

/* This is the endpoint receive callback that is executed in the secure domain. It ensures data is copied from secure
   shared memory to the non-secure memory and then it calls the application user-defined non-secure callback.  */
static int ept_read_cb_s(void *payload, int payload_len, unsigned long src, void *priv)
{
    struct rpmsg_lite_endpoint_callback_descr_ns *callback_descr = priv;

    env_memcpy((void *)callback_descr->rx_cb_data->app_buffer, payload, payload_len);

    /* Define non-secure callback function pointer with cleared LSB */
    rl_ept_rx_cb_ns_t rx_cb_ns = (rl_ept_rx_cb_ns_t)cmse_nsfptr_create(callback_descr->rx_cb);

    /* Calls the application user-defined non-secure callback */
    int return_value =
        rx_cb_ns(callback_descr->rx_cb_data->app_buffer, payload_len, src, callback_descr->rx_cb_data->app_data);

    return RL_RELEASE;
}

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
__cmse_nonsecure_entry struct rpmsg_lite_endpoint*
rpmsg_lite_create_ept_nse(unsigned long addr, struct rpmsg_lite_endpoint_callback_descr_ns *ept_callback_descr)

#else
__attribute__((cmse_nonsecure_entry)) struct rpmsg_lite_endpoint*
rpmsg_lite_create_ept_nse(unsigned long addr, struct rpmsg_lite_endpoint_callback_descr_ns *ept_callback_descr)
#endif
{
#if defined(RL_USE_STATIC_API) && (RL_USE_STATIC_API == 1)
    if (cmse_check_pointed_object(ept_callback_descr, CMSE_NONSECURE) != NULL)
    {
        return rpmsg_lite_create_ept(rpmsg_lite_instance_s, addr, ept_read_cb_s, ept_callback_descr,
                                     &ept_context_s);
    }
    else
    {
        PRINTF("ept_callback_descr is not located in normal world!\r\n");
        abort();
    }
#else
    if (cmse_check_pointed_object(ept_callback_descr, CMSE_NONSECURE) != NULL))
        {
            return rpmsg_lite_create_ept(rpmsg_lite_instance_s, addr, ept_read_cb_s, ept_callback_descr);
        }
    else
    {
        PRINTF("ept_callback_descr is not located in normal world!\r\n");
        abort();
    }
#endif
}
#if defined(__IAR_SYSTEMS_ICC__)
__cmse_nonsecure_entry int rpmsg_lite_destroy_ept_nse(struct rpmsg_lite_endpoint *rl_ept)
#else
__attribute__((cmse_nonsecure_entry)) int rpmsg_lite_destroy_ept_nse(struct rpmsg_lite_endpoint *rl_ept)
#endif
{
    return rpmsg_lite_destroy_ept(rpmsg_lite_instance_s, rl_ept);
}

#if defined(__IAR_SYSTEMS_ICC__)
__cmse_nonsecure_entry int rpmsg_lite_send_nse(struct rpmsg_lite_endpoint *ept,
                                                              struct rpmsg_lite_send_params_ns *message_params,
                                                              unsigned long timeout)
#else
__attribute__((cmse_nonsecure_entry)) int rpmsg_lite_send_nse(struct rpmsg_lite_endpoint *ept,
                                                              struct rpmsg_lite_send_params_ns *message_params,
                                                              unsigned long timeout)
#endif
{
    if (cmse_check_pointed_object(message_params, CMSE_NONSECURE) != NULL)
    {
        return rpmsg_lite_send(rpmsg_lite_instance_s, ept, message_params->dst, message_params->data,
                           message_params->size, timeout);
    }
    else
    {
        PRINTF("message_params is not located in normal world!\r\n");
        abort();
    }
}
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
