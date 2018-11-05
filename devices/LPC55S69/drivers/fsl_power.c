/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_common.h"
#include "fsl_power.h"

/*! @name Driver version */
/*@{*/
/*! @brief LPC546XX power contorl version 1.0.0. */
#define FSL_LPC55SXX_POWER_CONTROL_VERSION (MAKE_VERSION(1, 0, 0))
/*@}*/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static lowpower_driver_interface_t *s_lowpowerDriver = (lowpower_driver_interface_t *)(0x130010d4);

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpc_niobe4_power"
#endif
/*! @brief set and clear bit MACRO's. */
#define U32_SET_BITS(P, B) ((*(uint32_t *)P) |= (B))
#define U32_CLR_BITS(P, B) ((*(uint32_t *)P) &= ~(B))

/*******************************************************************************
 * Codes
 ******************************************************************************/

/*******************************************************************************
 * LOCAL FUNCTIONS PROTOTYPES
 ******************************************************************************/
static void lf_get_deepsleep_core_supply_cfg(uint32_t exclude_from_pd, uint32_t *core_supply, uint32_t *dcdc_voltage);

/**
 * @brief   Configures and enters in low power mode
 * @param           : p_lowpower_cfg
 * @return  Nothing
 */
void Power_EnterLowPower(LPC_LOWPOWER_T *p_lowpower_cfg)
{
#define FLASH_NMPA_GPO_0_2 0x9FC08UL
#define FLASH_NMPA_LDO_AO_ADDR 0x9FCF4UL

    /* TODO :: :: PMC clk (1 MHz or 12 MHZ) shall be set here */

    /* TODO :: :: Analog References Fast wake up setting shall be done here (enabled or disabled) */

    /* CPU Retention configuration : preserve the value of FUNCRETENTIONCTRL.RET_LENTH which is a Hardware defined
     * parameter */

    p_lowpower_cfg->CPURETCTRL = (SYSCON->FUNCRETENTIONCTRL & SYSCON_FUNCRETENTIONCTRL_RET_LENTH_MASK) |
                                 (p_lowpower_cfg->CPURETCTRL & (~SYSCON_FUNCRETENTIONCTRL_RET_LENTH_MASK));

    {
        /* TODO :: Didier C :: this code shall be removed from the official API */
        uint32_t flash_op_mode = (*((volatile uint32_t *)(FLASH_NMPA_GPO_0_2))) >> 2;
        if (flash_op_mode == 1)
        {
            /* Flash is operating with the RCLK : switch back RCLK from 96 MHz to 48 MHz */
            SYSCON->FLASHRCLKSEL = SYSCON_FLASHRCLKSEL_SEL(0);
        }
    }

    /* Switch System Clock to FRO12Mhz (the configuration before calling this function will not be restored back) */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);                  /* Switch main clock to FRO12MHz */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1, 0);               /* Main clock divided by 1 */
    SYSCON->FMCCR = (SYSCON->FMCCR & 0xFFFF0000) | 0x201A; /* Adjust FMC waiting time cycles */

    (*(s_lowpowerDriver->set_lowpower_mode))(p_lowpower_cfg);

    /* Restore the configuration of the MISCCTRL Register : LOWPWR_FLASH_BUF = 0, LDOMEMBLEEDDSLP = 0, LDOMEMHIGHZMODE =
     * 0 */
    PMC->MISCCTRL &= (~PMC_MISCCTRL_LOWPWR_FLASH_BUF_MASK) & (~PMC_MISCCTRL_DISABLE_BLEED_MASK) &
                     (~PMC_MISCCTRL_LDOMEMHIGHZMODE_MASK);
}

/**
 * @brief   Shut off the Flash and execute the _WFI(), then power up the Flash after wake-up event
 * @param   None
 * @return  Nothing
 */
void POWER_CycleCpuAndFlash(void)
{
    (*(s_lowpowerDriver->power_cycle_cpu_and_flash))();
};

/**
 * brief    PMC Deep Sleep function call
 * return   nothing
 */
void POWER_DeepSleep(uint32_t exclude_from_pd,
                     uint32_t sram_retention_ctrl,
                     uint64_t wakeup_interrupts,
                     uint32_t hardware_wake_ctrl)
{
    LPC_LOWPOWER_T lv_low_power_mode_cfg; /* Low Power Mode configuration structure */
    uint32_t cpu0_nmi_enable;
    uint32_t cpu0_int_enable_0;
    uint32_t cpu0_int_enable_1;
    uint32_t core_supply; /* 0 = LDO DEEPSLEEP / 1 = DCDC */
    uint32_t dcdc_voltage;

    /* Clear Low Power Mode configuration variable */
    memset(&lv_low_power_mode_cfg, 0x0, sizeof(LPC_LOWPOWER_T));

    /* Configure Low Power Mode configuration variable */
    lv_low_power_mode_cfg.CFG |= LOWPOWER_CFG_LPMODE_DEEPSLEEP << LOWPOWER_CFG_LPMODE_INDEX; /* DEEPSLEEP mode */

    lf_get_deepsleep_core_supply_cfg(exclude_from_pd, &core_supply, &dcdc_voltage);

    if (core_supply == 1)
    {
        /* DCDC will be used during Deep Sleep (instead of LDO Deep Sleep); Make sure LDO MEM & Analog references will
         * stay powered */
        lv_low_power_mode_cfg.PDCTRL0 =
            (~exclude_from_pd & ~kPDRUNCFG_PD_DCDC & ~kPDRUNCFG_PD_LDOMEM & ~kPDRUNCFG_PD_BIAS) |
            kPDRUNCFG_PD_LDODEEPSLEEP;
    }
    else
    {
        /* LDO DEEPSLEEP will be used during Deep Sleep (instead of DCDC); Make sure LDO MEM & Analog references will
         * stay powered */
        lv_low_power_mode_cfg.PDCTRL0 =
            (~exclude_from_pd & ~kPDRUNCFG_PD_LDODEEPSLEEP & ~kPDRUNCFG_PD_LDOMEM & ~kPDRUNCFG_PD_BIAS) |
            kPDRUNCFG_PD_DCDC;
    }

    /* Shut down ROM */
    lv_low_power_mode_cfg.PDCTRL0 |= kPDRUNCFG_PD_ROM;

    /* Voltage control in DeepSleep Low Power Modes */
    /* TODO :: :: The Memories Voltage settings below are valid for Source Biasing; for voltage scaling, they need to be
     * update */

    lv_low_power_mode_cfg.VOLTAGE = (V_AO_0P900 << LOWPOWER_VOLTAGE_LDO_PMU_INDEX) |       /* Should be set to 0.9 V */
                                    (V_AO_0P800 << LOWPOWER_VOLTAGE_LDO_PMU_BOOST_INDEX) | /* Should be set to 0.8 V  */
                                    (V_AO_1P100 << LOWPOWER_VOLTAGE_LDO_MEM_INDEX) |       /* Should be set to 1.1V */
                                    (V_AO_0P800 << LOWPOWER_VOLTAGE_LDO_MEM_BOOST_INDEX) | /* Should be set to 0.8V  */
                                    (V_DEEPSLEEP_0P900 << LOWPOWER_VOLTAGE_LDO_DEEP_SLEEP_INDEX) | /* Set to 0.90 V */
                                    (dcdc_voltage << LOWPOWER_VOLTAGE_DCDC_INDEX)                  /* */
        ;

    /* SRAM retention control during POWERDOWN */
    lv_low_power_mode_cfg.SRAMRETCTRL = sram_retention_ctrl;

    /* CPU Wake up & Interrupt sources control */
    lv_low_power_mode_cfg.WAKEUPINT = wakeup_interrupts;
    lv_low_power_mode_cfg.WAKEUPSRC = wakeup_interrupts;

    /* Interrupts that allow DMA transfers with Flexcomm without waking up the Processor */
    lv_low_power_mode_cfg.HWWAKE = hardware_wake_ctrl;

    cpu0_nmi_enable = SYSCON->NMISRC & SYSCON_NMISRC_NMIENCPU0_MASK; /* Save the configuration of the NMI Register */
    SYSCON->NMISRC &= ~SYSCON_NMISRC_NMIENCPU0_MASK;                 /* Disable NMI of CPU0 */

    /* Save the configuration of the CPU interrupt enable Registers (because they are overwritten inside the low power
     * API */
    cpu0_int_enable_0 = NVIC->ISER[0];
    cpu0_int_enable_1 = NVIC->ISER[1];

    /* TODO :: :: Shall BOD Core be disabled ???? */

    /* Enter low power mode */
    Power_EnterLowPower(&lv_low_power_mode_cfg);

    /* TODO :: :: Shall BOD Core be re-enabled ???? */

    /* Restore the configuration of the NMI Register */
    SYSCON->NMISRC |= cpu0_nmi_enable;

    /* Restore the configuration of the CPU interrupt enable Registers (because they have been overwritten inside the
     * low power API */
    NVIC->ISER[0] = cpu0_int_enable_0;
    NVIC->ISER[1] = cpu0_int_enable_1;
}

/**
 * brief    PMC power Down function call
 * return   nothing
 */
void POWER_PowerDown(uint32_t exclude_from_pd,
                     uint32_t sram_retention_ctrl,
                     uint64_t wakeup_interrupts,
                     uint32_t cpu_retention_ctrl)
{
    LPC_LOWPOWER_T lv_low_power_mode_cfg; /* Low Power Mode configuration structure */
    uint32_t cpu0_nmi_enable;
    uint32_t cpu0_int_enable_0;
    uint32_t cpu0_int_enable_1;
    uint64_t wakeup_src_int;

    /* Clear Low Power Mode configuration variable */
    memset(&lv_low_power_mode_cfg, 0x0, sizeof(LPC_LOWPOWER_T));

    /* Configure Low Power Mode configuration variable */
    lv_low_power_mode_cfg.CFG |= LOWPOWER_CFG_LPMODE_POWERDOWN << LOWPOWER_CFG_LPMODE_INDEX; /* POWER DOWN mode */

    /* Only FRO32K, XTAL32K, COMP, BIAS and LDO_MEM can be stay powered during POWERDOWN (valid from application point
     * of view; Hardware allows BODVBAT, LDODEEPSLEEP and FRO1M to stay powered, that's why they are excluded below) */
    lv_low_power_mode_cfg.PDCTRL0 =
        (~exclude_from_pd) | kPDRUNCFG_PD_BODVBAT | kPDRUNCFG_PD_FRO1M | kPDRUNCFG_PD_LDODEEPSLEEP;

    /* SRAM retention control during POWERDOWN */
    lv_low_power_mode_cfg.SRAMRETCTRL = sram_retention_ctrl;

    /* Sanity check: If retention is required for any of SRAM instances, make sure LDO MEM will stay powered */
    if ((sram_retention_ctrl & 0x7FFF) != 0)
    {
        lv_low_power_mode_cfg.PDCTRL0 &= ~kPDRUNCFG_PD_LDOMEM;
    }

    /* Voltage control in Low Power Modes */
    /* TODO :: :: The Memories Voltage settings below are valid for Source Biasing; for voltage scaling, they need to be
     * update */
    lv_low_power_mode_cfg.VOLTAGE = (V_AO_0P800 << LOWPOWER_VOLTAGE_LDO_PMU_INDEX) |       /* Set to 0.80 V */
                                    (V_AO_0P750 << LOWPOWER_VOLTAGE_LDO_PMU_BOOST_INDEX) | /* Set to 0.75 V */
                                    (V_AO_1P100 << LOWPOWER_VOLTAGE_LDO_MEM_INDEX) |       /* Set to 1.10 V */
                                    (V_AO_1P050 << LOWPOWER_VOLTAGE_LDO_MEM_BOOST_INDEX)   /* Set to 1.05 V */
        ;

    /* CPU0 retention Ctrl.
     * For the time being, we do not allow customer to relocate the CPU retention area in SRAMX, meaning that the
     * retention area range is [0x0400_6000 - 0x0400_6600] (beginning of RAMX2)
     * If required by customer, cpu_retention_ctrl[13:1] will be used for that to modify the default retention area
     */
    lv_low_power_mode_cfg.CPURETCTRL = (cpu_retention_ctrl & 0x1) << LOWPOWER_CPURETCTRL_ENA_INDEX;

    /* CPU Wake up & Interrupt sources control : only WAKEUP_GPIO_GLOBALINT0, WAKEUP_GPIO_GLOBALINT1, WAKEUP_FLEXCOMM3,
     * WAKEUP_ACMP_CAPT, WAKEUP_RTC_LITE_ALARM_WAKEUP, WAKEUP_OS_EVENT_TIMER */
    wakeup_src_int = (uint64_t)(WAKEUP_GPIO_GLOBALINT0 | WAKEUP_GPIO_GLOBALINT1 | WAKEUP_FLEXCOMM3 | WAKEUP_ACMP_CAPT |
                                WAKEUP_RTC_LITE_ALARM_WAKEUP | WAKEUP_OS_EVENT_TIMER);
    lv_low_power_mode_cfg.WAKEUPINT = wakeup_interrupts & wakeup_src_int;
    lv_low_power_mode_cfg.WAKEUPSRC = wakeup_interrupts & wakeup_src_int;

    cpu0_nmi_enable = SYSCON->NMISRC & SYSCON_NMISRC_NMIENCPU0_MASK; /* Save the configuration of the NMI Register */
    SYSCON->NMISRC &= ~SYSCON_NMISRC_NMIENCPU0_MASK;                 /* Disable NMI of CPU0 */

    /* Save the configuration of the CPU interrupt enable Registers (because they are overwritten inside the low power
     * API */
    cpu0_int_enable_0 = NVIC->ISER[0];
    cpu0_int_enable_1 = NVIC->ISER[1];

    /* Enter low power mode */
    Power_EnterLowPower(&lv_low_power_mode_cfg);

    /*** We'll reach this point in case of POWERDOWN with CPU retention or if the POWERDOWN has not been taken (for
       instance because an interrupt is pending).
        In case of CPU retention, assumption is that the SRAM containing the stack used to call this function shall be
       preserved during low power ***/

    /* Restore the configuration of the NMI Register */
    SYSCON->NMISRC |= cpu0_nmi_enable;

    /* Restore the configuration of the CPU interrupt enable Registers (because they have been overwritten inside the
     * low power API */
    NVIC->ISER[0] = cpu0_int_enable_0;
    NVIC->ISER[1] = cpu0_int_enable_1;
}

/**
 * brief    PMC Deep Sleep Power Down function call
 * return   nothing
 */
void POWER_DeepPowerDown(uint32_t exclude_from_pd,
                         uint32_t sram_retention_ctrl,
                         uint64_t wakeup_interrupts,
                         uint32_t wakeup_io_ctrl)
{
    LPC_LOWPOWER_T lv_low_power_mode_cfg; /* Low Power Mode configuration structure */
    uint32_t cpu0_nmi_enable;
    uint32_t cpu0_int_enable_0;
    uint32_t cpu0_int_enable_1;

    /* Clear Low Power Mode configuration variable */
    memset(&lv_low_power_mode_cfg, 0x0, sizeof(LPC_LOWPOWER_T));

    /* Configure Low Power Mode configuration variable */
    lv_low_power_mode_cfg.CFG |= LOWPOWER_CFG_LPMODE_DEEPPOWERDOWN
                                 << LOWPOWER_CFG_LPMODE_INDEX; /* DEEP POWER DOWN mode */

    /* Only FRO32K, XTAL32K and LDO_MEM can be stay powered during DEEPPOWERDOWN (valid from application point of view;
     * Hardware allows BODVBAT, BIAS FRO1M and COMP to stay powered, that's why they are excluded below) */
    lv_low_power_mode_cfg.PDCTRL0 =
        (~exclude_from_pd) | kPDRUNCFG_PD_BIAS | kPDRUNCFG_PD_BODVBAT | kPDRUNCFG_PD_FRO1M | kPDRUNCFG_PD_COMP;

    /* SRAM retention control during DEEPPOWERDOWN */
    lv_low_power_mode_cfg.SRAMRETCTRL = sram_retention_ctrl;

    /* Sanity check: If retention is required for any of SRAM instances, make sure LDO MEM will stay powered */
    if ((sram_retention_ctrl & 0x7FFF) != 0)
    {
        lv_low_power_mode_cfg.PDCTRL0 &= ~kPDRUNCFG_PD_LDOMEM;
    }

    /* Voltage control in Low Power Modes */
    /* TODO :: :: The Memories Voltage settings below are valid for Source Biasing; for voltage scaling, they need to be
     * update */
    lv_low_power_mode_cfg.VOLTAGE = (V_AO_0P800 << LOWPOWER_VOLTAGE_LDO_PMU_INDEX) |       /* Set to 0.80 V */
                                    (V_AO_0P750 << LOWPOWER_VOLTAGE_LDO_PMU_BOOST_INDEX) | /* Set to 0.75 V */
                                    (V_AO_1P100 << LOWPOWER_VOLTAGE_LDO_MEM_INDEX) |       /* Set to 1.10 V */
                                    (V_AO_1P050 << LOWPOWER_VOLTAGE_LDO_MEM_BOOST_INDEX)   /* Set to 1.05 V */
        ;

    lv_low_power_mode_cfg.WAKEUPINT =
        wakeup_interrupts & (WAKEUP_RTC_LITE_ALARM_WAKEUP | WAKEUP_OS_EVENT_TIMER); /* CPU Wake up sources control :
                                                                                       only
                                                                                       WAKEUP_RTC_LITE_ALARM_WAKEUP,
                                                                                       WAKEUP_OS_EVENT_TIMER */
    lv_low_power_mode_cfg.WAKEUPSRC =
        wakeup_interrupts & (WAKEUP_RTC_LITE_ALARM_WAKEUP | WAKEUP_OS_EVENT_TIMER |
                             WAKEUP_ALLWAKEUPIOS); /*!< Hardware Wake up sources control: : only
                                                      WAKEUP_RTC_LITE_ALARM_WAKEUP, WAKEUP_OS_EVENT_TIMER and
                                                      WAKEUP_ALLWAKEUPIOS */

    /* Wake up I/O sources */
    lv_low_power_mode_cfg.WAKEUPIOSRC = wakeup_io_ctrl;

    cpu0_nmi_enable = SYSCON->NMISRC & SYSCON_NMISRC_NMIENCPU0_MASK; /* Save the configuration of the NMI Register */
    SYSCON->NMISRC &= ~SYSCON_NMISRC_NMIENCPU0_MASK;                 /* Disable NMI of CPU0 */

    /* Save the configuration of the CPU interrupt enable Registers (because they are overwritten inside the low power
     * API */
    cpu0_int_enable_0 = NVIC->ISER[0];
    cpu0_int_enable_1 = NVIC->ISER[1];

    /* Enter low power mode */
    Power_EnterLowPower(&lv_low_power_mode_cfg);

    /*** We'll reach this point ONLY and ONLY if the DEEPPOWERDOWN has not been taken (for instance because an RTC or
     * OSTIMER interrupt is pending) ***/

    /* Restore the configuration of the NMI Register */
    SYSCON->NMISRC |= cpu0_nmi_enable;

    /* Restore the configuration of the CPU interrupt enable Registers (because they have been overwritten inside the
     * low power API */
    NVIC->ISER[0] = cpu0_int_enable_0;
    NVIC->ISER[1] = cpu0_int_enable_1;
}

/**
 * brief    PMC Sleep function call
 * return   nothing
 */
void POWER_EnterSleep(void)
{
    uint32_t pmsk;
    pmsk = __get_PRIMASK();
    __disable_irq();
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
    __set_PRIMASK(pmsk);
}

/**
 * @brief    PMC Deep Sleep function call
 * @return   nothing
 */
void POWER_EnterDeepSleep(uint32_t exclude_from_pd)
{
    POWER_DeepSleep(exclude_from_pd, 0x0, WAKEUP_GPIO_INT0_0, 0x0);
}

/**
 * @brief    PMC power Down function call
 * @return   nothing
 */
void POWER_EnterPowerDown(uint32_t exclude_from_pd)
{
    POWER_PowerDown(exclude_from_pd, 0x0, WAKEUP_GPIO_INT0_0, 0x0);
}

/**
 * @brief    PMC Deep Power Down function call
 * @return   nothing
 */
void POWER_EnterDeepPowerDown(uint32_t exclude_from_pd)
{
    POWER_DeepPowerDown(exclude_from_pd, 0x0, WAKEUP_RTC_LITE_ALARM_WAKEUP, 0x0);
}
/**
 * @brief   Get Digital Core logic supply source to be used during Deep Sleep.
 * @param [in]  exclude_from_pd: COmpoenents NOT to be powered down during Deep Sleep
 * @param [out] core_supply: 0 = LDO DEEPSLEEP will be used / 1 = DCDC will be used
 * @param [out] dcdc_voltage: as defined by V_DCDC_* in niobe_lowpower.h

 * @return  Nothing
 */
static void lf_get_deepsleep_core_supply_cfg(uint32_t exclude_from_pd, uint32_t *core_supply, uint32_t *dcdc_voltage)
{
    *core_supply = 0;             /* LDO DEEPSLEEP */
    *dcdc_voltage = V_DCDC_0P950; /* Don't care, so put it to the minimum */

    /* TODO :: :: There might have some other conditions for which it would be more effiscient to use the DCDC
                 instead of LDO DEESLEEP (for instance, in case of high temperature, LDO DEESLEEP might not be strong
                 enough to supply
                 the required (high leakage) current. This NEED to be clarify during IC validation. */
    if (((exclude_from_pd & kPDRUNCFG_PD_USB1_PHY) == 0) && ((exclude_from_pd & kPDRUNCFG_PD_LDOUSBHS) == 0))
    {
        /* USB High Speed is required as wake-up source in Deep Sleep mode: */
        PMC->MISCCTRL |= PMC_MISCCTRL_LOWPWR_FLASH_BUF_MASK; /* Force flash buffer in low power mode */
        *core_supply = 1;                                    /* DCDC */
        *dcdc_voltage = V_DCDC_1P025; /* Set DCDC voltage to be 1.025 V (USB HS IP cannot work below 0.990 V) */
    }
}

/* Enter Power mode */
/*!
 * brief Power Library API to enter different power mode.
 *
 * param exclude_from_pd  Bit mask of the PDRUNCFG0(low 32bits) and PDRUNCFG1(high 32bits) that needs to be powered on
 * during power mode selected.
 * return none
 */
void POWER_EnterPowerMode(power_mode_cfg_t mode, uint64_t exclude_from_pd)
{
    switch (mode)
    {
        case kPmu_Sleep:
            POWER_EnterSleep();
            break;

        case kPmu_Deep_Sleep:
            POWER_EnterDeepSleep(exclude_from_pd);
            break;

        case kPmu_PowerDown:
            POWER_EnterPowerDown(exclude_from_pd);
            break;

        case kPmu_Deep_PowerDown:
            POWER_EnterDeepPowerDown(exclude_from_pd);
            break;

        default:
            break;
    }
}

/* Get power lib version */
/*!
 * brief Power Library API to return the library version.
 *
 * param none
 * return version number of the power library
 */
uint32_t POWER_GetLibVersion(void)
{
    return FSL_LPC55SXX_POWER_CONTROL_VERSION;
}
