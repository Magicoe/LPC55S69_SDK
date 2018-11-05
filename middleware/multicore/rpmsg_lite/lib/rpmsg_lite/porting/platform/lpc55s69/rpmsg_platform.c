/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "rpmsg_platform.h"
#include "rpmsg_env.h"

#include "fsl_device_registers.h"
#include "fsl_mailbox.h"

#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
#include "mcmgr.h"
#endif

static int isr_counter = 0;
static int disable_counter = 0;
static void *lock;

#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
static void mcmgr_event_handler(uint16_t vring_idx, void *context)
{
    env_isr(vring_idx);
}
#else
void MAILBOX_IRQHandler(void)
{
    mailbox_cpu_id_t cpu_id;
#if defined(FSL_FEATURE_MAILBOX_SIDE_A)
    cpu_id = kMAILBOX_CM33_Core0;
#else
    cpu_id = kMAILBOX_CM33_Core1;
#endif

    uint32_t value = MAILBOX_GetValue(MAILBOX, cpu_id);

    if (value & 0x01)
    {
        MAILBOX_ClearValueBits(MAILBOX, cpu_id, 0x01);
        env_isr(0);
    }
    if (value & 0x02)
    {
        MAILBOX_ClearValueBits(MAILBOX, cpu_id, 0x02);
        env_isr(1);
    }
}
#endif

int platform_init_interrupt(int vq_id, void *isr_data)
{
    /* Register ISR to environment layer */
    env_register_isr(vq_id, isr_data);

    env_lock_mutex(lock);

    assert(0 <= isr_counter);
    if (!isr_counter)

    NVIC_SetPriority(MAILBOX_IRQn, 5);
    isr_counter++;

    env_unlock_mutex(lock);

    return 0;
}

int platform_deinit_interrupt(int vq_id)
{
    /* Prepare the MU Hardware */
    env_lock_mutex(lock);

    assert(0 < isr_counter);
    isr_counter--;
    if (!isr_counter)
        NVIC_DisableIRQ(MAILBOX_IRQn);

    /* Unregister ISR from environment layer */
    env_unregister_isr(vq_id);

    env_unlock_mutex(lock);

    return 0;
}

void platform_notify(int vq_id)
{
#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
    env_lock_mutex(lock);
    MCMGR_TriggerEventForce(kMCMGR_RemoteRPMsgEvent, RL_GET_Q_ID(vq_id));
    env_unlock_mutex(lock);
#else
/* Only single RPMsg-Lite instance (LINK_ID) is defined for this dual core device. Extend
   this statement in case multiple instances of RPMsg-Lite are needed. */
    switch (RL_GET_LINK_ID(vq_id))
    {
        case RL_PLATFORM_LPC55S69_M33_M33_LINK_ID:
            env_lock_mutex(lock);
/* Write directly into the Mailbox register, no need to wait until the content is cleared
   (consumed by the receiver side) because the same walue of the virtqueu ID is written
   into this register when trigerring the ISR for the receiver side. The whole queue of
   received buffers for associated virtqueue is handled in the ISR then. */
#if defined(FSL_FEATURE_MAILBOX_SIDE_A)
            MAILBOX_SetValueBits(MAILBOX, kMAILBOX_CM33_Core1, (1 << RL_GET_Q_ID(vq_id)));
#else
            MAILBOX_SetValueBits(MAILBOX, kMAILBOX_CM33_Core0, (1 << RL_GET_Q_ID(vq_id)));
#endif
            env_unlock_mutex(lock);
            return;

        default:
            return;
    }
#endif
}

/**
 * platform_time_delay
 *
 * @param num_msec Delay time in ms.
 *
 * This is not an accurate delay, it ensures at least num_msec passed when return.
 */
void platform_time_delay(int num_msec)
{
    uint32_t loop;

    /* Recalculate the CPU frequency */
    SystemCoreClockUpdate();

    /* Calculate the CPU loops to delay, each loop has 3 cycles */
    loop = SystemCoreClock / 3 / 1000 * num_msec;

    /* There's some difference among toolchains, 3 or 4 cycles each loop */
    while (loop)
    {
        __NOP();
        loop--;
    }
}

/**
 * platform_in_isr
 *
 * Return whether CPU is processing IRQ
 *
 * @return True for IRQ, false otherwise.
 *
 */
int platform_in_isr(void)
{
    return ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0);
}

/**
 * platform_interrupt_enable
 *
 * Enable peripheral-related interrupt with passed priority and type.
 *
 * @param vq_id Vector ID that need to be converted to IRQ number
 * @param trigger_type IRQ active level
 * @param trigger_type IRQ priority
 *
 * @return vq_id. Return value is never checked..
 *
 */
int platform_interrupt_enable(unsigned int vq_id)
{
    assert(0 < disable_counter);

    __asm volatile("cpsid i");
    disable_counter--;

    if (!disable_counter)
        NVIC_EnableIRQ(MAILBOX_IRQn);
    __asm volatile("cpsie i");
    return (vq_id);
}

/**
 * platform_interrupt_disable
 *
 * Disable peripheral-related interrupt.
 *
 * @param vq_id Vector ID that need to be converted to IRQ number
 *
 * @return vq_id. Return value is never checked.
 *
 */
int platform_interrupt_disable(unsigned int vq_id)
{
    assert(0 <= disable_counter);

    __asm volatile("cpsid i");
    // virtqueues use the same NVIC vector
    // if counter is set - the interrupts are disabled
    if (!disable_counter)
        NVIC_DisableIRQ(MAILBOX_IRQn);

    disable_counter++;
    __asm volatile("cpsie i");
    return (vq_id);
}

/**
 * platform_map_mem_region
 *
 * Dummy implementation
 *
 */
void platform_map_mem_region(unsigned int vrt_addr, unsigned int phy_addr, unsigned int size, unsigned int flags)
{
}

/**
 * platform_cache_all_flush_invalidate
 *
 * Dummy implementation
 *
 */
void platform_cache_all_flush_invalidate()
{
}

/**
 * platform_cache_disable
 *
 * Dummy implementation
 *
 */
void platform_cache_disable()
{
}

/**
 * platform_vatopa
 *
 * Dummy implementation
 *
 */
unsigned long platform_vatopa(void *addr)
{
    return ((unsigned long)addr);
}

/**
 * platform_patova
 *
 * Dummy implementation
 *
 */
void *platform_patova(unsigned long addr)
{
    return ((void *)addr);
}

/**
 * platform_init
 *
 * platform/environment init
 */
int platform_init(void)
{
#if defined(RL_USE_MCMGR_IPC_ISR_HANDLER) && (RL_USE_MCMGR_IPC_ISR_HANDLER == 1)
    MCMGR_RegisterEvent(kMCMGR_RemoteRPMsgEvent, mcmgr_event_handler, NULL);
#else
    MAILBOX_Init(MAILBOX);
#endif

    /* Create lock used in multi-instanced RPMsg */
    env_create_mutex(&lock, 1);

    return 0;
}

/**
 * platform_deinit
 *
 * platform/environment deinit process
 */
int platform_deinit(void)
{
/* Important for LPC5411x - do not deinit mailbox, if there
   is a pending ISR on the other core! */
#if defined(FSL_FEATURE_MAILBOX_SIDE_A)
    while (0 != MAILBOX_GetValue(MAILBOX, kMAILBOX_CM33_Core1))
        ;
#else
    while (0 != MAILBOX_GetValue(MAILBOX, kMAILBOX_CM33_Core0))
        ;
#endif

    MAILBOX_Deinit(MAILBOX);

    /* Delete lock used in multi-instanced RPMsg */
    env_delete_mutex(lock);
    lock = NULL;
    return 0;
}
