/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_host_config.h"
#include "usb_host.h"
#include "fsl_device_registers.h"
#include "usb_host_hid.h"
#include "board.h"
#include "host_mouse.h"
#include "pin_mux.h"
#include "fsl_common.h"
#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */
#include "app.h"
#include "board.h"

#include "usb_io.h"
#include "usb_timer.h"
#include "stdlib.h"
#include "fsl_debug_console.h"
#if ((!USB_HOST_CONFIG_KHCI) && (!USB_HOST_CONFIG_EHCI) && (!USB_HOST_CONFIG_OHCI) && (!USB_HOST_CONFIG_IP3516HS))
#error Please enable USB_HOST_CONFIG_KHCI, USB_HOST_CONFIG_EHCI, USB_HOST_CONFIG_OHCI, or USB_HOST_CONFIG_IP3516HS in file usb_host_config.
#endif

#include "usb_phy.h"
#include <stdbool.h>
#include "fsl_power.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if ((defined(USB_HOST_CONFIG_OHCI)) && (USB_HOST_CONFIG_OHCI > 0U))
#define APP_EXCLUDE_FROM_DEEPSLEEP                                                              \
    (kPDRUNCFG_PD_LDOFLASHNV | kPDRUNCFG_PD_USB1_PHY | kPDRUNCFG_PD_DCDC | kPDRUNCFG_PD_GPADC | \
     kPDRUNCFG_PD_LDOGPADC | kPDRUNCFG_PD_FRO192M | kPDRUNCFG_PD_PLL0 | kPDRUNCFG_PD_PLL1 | kPDRUNCFG_PD_XTAL32M)
#define APP_EXCLUDE_FROM_DEEPSLEEP1 0U
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS)) && (USB_HOST_CONFIG_IP3516HS > 0U))
#define APP_EXCLUDE_FROM_DEEPSLEEP                                                              \
    (kPDRUNCFG_PD_LDOFLASHNV | kPDRUNCFG_PD_USB1_PHY | kPDRUNCFG_PD_DCDC | kPDRUNCFG_PD_GPADC | \
     kPDRUNCFG_PD_LDOGPADC | kPDRUNCFG_PD_FRO192M | kPDRUNCFG_PD_PLL0 | kPDRUNCFG_PD_PLL1 | kPDRUNCFG_PD_XTAL32M)

#define APP_EXCLUDE_FROM_DEEPSLEEP1 ((PMC_PDRUNCFG0_PDEN_USBHSPHY_MASK | PMC_PDRUNCFG0_PDEN_FRO192M_MASK))
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief host callback function.
 *
 * device attach/detach callback function.
 *
 * @param deviceHandle          device handle.
 * @param configurationHandle   attached device's configuration descriptor information.
 * @param eventCode             callback event code, please reference to enumeration host_event_t.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_NotSupported         The application don't support the configuration.
 */
static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode);

/*!
 * @brief application initialization.
 */
static void USB_HostApplicationInit(void);

extern void USB_HostClockInit(void);
extern void USB_HostIsrEnable(void);
extern void USB_HostTaskFn(void *param);
void BOARD_InitHardware(void);

status_t DbgConsole_Deinit(void);

void BOARD_InitPins(void);
void BOARD_DeinitPins(void);
void SW_IntControl(uint8_t enable);
char *SW_GetName(void);
void HW_TimerControl(uint8_t enable);
void USB_LowpowerModeInit(void);
void USB_PreLowpowerMode(void);
uint8_t USB_EnterLowpowerMode(void);
void USB_PostLowpowerMode(void);
void USB_ControllerSuspended(void);
void USB_WaitClockLocked(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern usb_host_mouse_instance_t g_HostHidMouse;
extern usb_host_handle g_HostHandle;
/*! @brief USB host mouse instance global variable */
extern usb_host_mouse_instance_t g_HostHidMouse;
usb_host_handle g_HostHandle;

/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_DeinitPins(void)
{
}

void SW_IntControl(uint8_t enable)
{
    if (enable)
    {
        g_HostHidMouse.selfWakeup = 0U;
    }
    USB_GpioInt(0, enable);
}

void SW_Callback(void)
{
    g_HostHidMouse.selfWakeup = 1U;
    SW_IntControl(0);
}

void SW_Init(void)
{
    USB_GpioInit(0, BOARD_SW1_GPIO_PORT, 1U << BOARD_SW1_GPIO_PIN, SW_Callback);
}

char *SW_GetName(void)
{
    return BOARD_SW1_NAME;
}

void HW_TimerCallback(void)
{
    g_HostHidMouse.hwTick++;
    USB_HostUpdateHwTick(g_HostHandle, g_HostHidMouse.hwTick);
}

void HW_TimerInit(void)
{
    USB_TimerInit(0, 1000U, CLOCK_GetFreq(kCLOCK_BusClk), HW_TimerCallback);
}

void HW_TimerControl(uint8_t enable)
{
    USB_TimerInt(0, enable);
}

void USB_LowpowerModeInit(void)
{
    SW_Init();
    HW_TimerInit();
}

void USB_PreLowpowerMode(void)
{
    __disable_irq();
#if ((defined(USB_HOST_CONFIG_OHCI)) && (USB_HOST_CONFIG_OHCI > 0U))
    NVIC_ClearPendingIRQ(USB0_NEEDCLK_IRQn);
    EnableDeepSleepIRQ(USB0_NEEDCLK_IRQn);
    SYSCON->STARTER[0] |= SYSCON_STARTER_USB0_NEEDCLK_MASK;
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS)) && (USB_HOST_CONFIG_IP3516HS > 0U))
    if (SYSCON->USB1CLKSTAT & (SYSCON_USB1CLKSTAT_DEV_NEED_CLKST_MASK))
    {
        /* enable usb1 device clock */
        CLOCK_EnableClock(kCLOCK_Usbd1);
        while (SYSCON->USB1CLKSTAT & (SYSCON_USB1CLKSTAT_DEV_NEED_CLKST_MASK))
        {
            __ASM("nop");
        }
        /* disable usb1 device clock */
        CLOCK_DisableClock(kCLOCK_Usbd1);
    }
    NVIC_ClearPendingIRQ(USB1_NEEDCLK_IRQn);
    EnableDeepSleepIRQ(USB1_NEEDCLK_IRQn);
    SYSCON->STARTER[1] |= SYSCON_STARTER_USB1_NEEDCLK_MASK;
#endif

#if 0
    CLOCK_AttachClk(
        kFRO12M_to_MAIN_CLK);          /*!< Switch to 12MHz first to ensure we can change voltage without accidentally
                                       being below the voltage for current speed */
    SYSCON->FROCTRL &= ~(SYSCON_FROCTRL_USBCLKADJ_MASK | SYSCON_FROCTRL_HSPDCLK_MASK);
    POWER_SetVoltageForFreq(12000000U); /*!< Set voltage for core */
#endif
#if ((defined(USB_HOST_CONFIG_OHCI)) && (USB_HOST_CONFIG_OHCI > 0U))
/* FS USB host does not support LPM */
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS)) && (USB_HOST_CONFIG_IP3516HS > 0U))

    SYSCON->MAINCLKSELA = 0;
#endif
}

uint8_t USB_EnterLowpowerMode(void)
{
    /* Enter Sleep mode */
    POWER_EnterSleep();
    return kStatus_Success;
}

void USB_PostLowpowerMode(void)
{
    __enable_irq();
#if ((defined(USB_HOST_CONFIG_OHCI)) && (USB_HOST_CONFIG_OHCI > 0U))
    DisableDeepSleepIRQ(USB0_NEEDCLK_IRQn);
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS)) && (USB_HOST_CONFIG_IP3516HS > 0U))
    DisableDeepSleepIRQ(USB1_NEEDCLK_IRQn);
#endif

#if 0
    BOARD_BootClockFROHF96M();
#endif
}

void USB_ControllerSuspended(void)
{
#if ((defined(USB_HOST_CONFIG_OHCI)) && (USB_HOST_CONFIG_OHCI > 0U))
    while (SYSCON->USB0CLKSTAT & (SYSCON_USB0CLKSTAT_HOST_NEED_CLKST_MASK))
    {
        __ASM("nop");
    }
    SYSCON->USB0CLKCTRL |= SYSCON_USB0CLKCTRL_POL_FS_HOST_CLK_MASK;
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS)) && (USB_HOST_CONFIG_IP3516HS > 0U))
    while (SYSCON->USB1CLKSTAT & (SYSCON_USB1CLKSTAT_HOST_NEED_CLKST_MASK))
    {
        __ASM("nop");
    }
    SYSCON->USB1CLKCTRL |= SYSCON_USB1CLKCTRL_POL_HS_HOST_CLK_MASK;
#endif
}

void USB0_NEEDCLK_IRQHandler(void)
{
}

void USB1_NEEDCLK_IRQHandler(void)
{
}

void USB_WaitClockLocked(void)
{
#if ((defined(USB_HOST_CONFIG_OHCI)) && (USB_HOST_CONFIG_OHCI > 0U))
/* FS USB host does not support LPM */
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS)) && (USB_HOST_CONFIG_IP3516HS > 0U))
    SYSCON->MAINCLKSELA = 3; /* only applies to BOARD_BootClockFROHF96M */
#endif
#if ((defined(USB_HOST_CONFIG_IP3516HS)) && (USB_HOST_CONFIG_IP3516HS > 0U))
    if ((SYSCON->USB1CLKSEL & SYSCON_USB1CLKSEL_SEL_MASK) == SYSCON_USB1CLKSEL_SEL(0x2u))
    {
        while (CLOCK_IsPLL1Locked() == false)
        {
        }
    }
#endif
}

#if (defined(USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI > 0U))
void USB0_IRQHandler(void)
{
    USB_HostOhciIsrFunction(g_HostHandle);
}
#endif /* USB_HOST_CONFIG_OHCI */
#if (defined(USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS > 0U))
void USB1_IRQHandler(void)
{
    USB_HostIp3516HsIsrFunction(g_HostHandle);
}
#endif /* USB_HOST_CONFIG_IP3516HS */

void USB_HostClockInit(void)
{
#if ((defined USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI > 0U))
    CLOCK_EnableUsbfs0HostClock(kCLOCK_UsbfsSrcFro, CLOCK_GetFroHfFreq());
#if ((defined FSL_FEATURE_USBFSH_USB_RAM) && (FSL_FEATURE_USBFSH_USB_RAM > 0U))
    for (int i = 0; i < (FSL_FEATURE_USBFSH_USB_RAM >> 2); i++)
    {
        ((uint32_t *)FSL_FEATURE_USBFSH_USB_RAM_BASE_ADDRESS)[i] = 0U;
    }
#endif
#endif /* USB_HOST_CONFIG_OHCI */

#if ((defined USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS > 0U))
    CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_UsbPhySrcExt, BOARD_XTAL0_CLK_HZ);
    CLOCK_EnableUsbhs0HostClock(kCLOCK_UsbSrcUnused, 0U);
    USB_EhciLowPowerPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, NULL);
#if ((defined FSL_FEATURE_USBHSH_USB_RAM) && (FSL_FEATURE_USBHSH_USB_RAM > 0U))
    for (int i = 0; i < (FSL_FEATURE_USBHSH_USB_RAM >> 2); i++)
    {
        ((uint32_t *)FSL_FEATURE_USBHSH_USB_RAM_BASE_ADDRESS)[i] = 0U;
    }
#endif
#endif /* USB_HOST_CONFIG_IP3511HS */
}

void USB_HostIsrEnable(void)
{
    uint8_t irqNumber;
#if ((defined USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI > 0U))
    IRQn_Type usbHsIrqs[] = {(IRQn_Type)USB0_IRQn};
    irqNumber = usbHsIrqs[CONTROLLER_ID - kUSB_ControllerOhci0];
#endif /* USB_HOST_CONFIG_OHCI */
#if ((defined USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS > 0U))
    IRQn_Type usbHsIrqs[] = {(IRQn_Type)USB1_IRQn};
    irqNumber = usbHsIrqs[CONTROLLER_ID - kUSB_ControllerIp3516Hs0];
#endif /* USB_HOST_CONFIG_IP3511HS */

/* Install isr, set priority, and enable IRQ. */
#if defined(__GIC_PRIO_BITS)
    GIC_SetPriority((IRQn_Type)irqNumber, USB_HOST_INTERRUPT_PRIORITY);
#else
    NVIC_SetPriority((IRQn_Type)irqNumber, USB_HOST_INTERRUPT_PRIORITY);
#endif
    EnableIRQ((IRQn_Type)irqNumber);
}

void USB_HostTaskFn(void *param)
{
#if ((defined USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI > 0U))
    USB_HostOhciTaskFunction(param);
#endif /* USB_HOST_CONFIG_OHCI */
#if ((defined USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS > 0U))
    USB_HostIp3516HsTaskFunction(param);
#endif /* USB_HOST_CONFIG_IP3516HS */
}

/*!
 * @brief USB isr function.
 */
void *USB_AppMemoryAllocate(uint32_t length)
{
    void *p = (void *)malloc(length);
    uint8_t *temp = (uint8_t *)p;
    if (p)
    {
        for (uint32_t count = 0U; count < length; count++)
        {
            temp[count] = 0U;
        }
    }
    return p;
}
void USB_AppMemoryFree(void *p)
{
    free(p);
}
static void USB_HostHidControlGetBOSCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_mouse_instance_t *mouseInstance = &g_HostHidMouse;
    if (NULL == param)
    {
        return;
    }
    USB_HostFreeTransfer(param, transfer);
    if (mouseInstance->L1sleepResumeState ==
        kUSB_HostRunWaitGetBOSDescriptor5) /* get the first five byte of bos descriptor finish */
    {
        if (kStatus_USB_Success == status)
        {
            mouseInstance->L1sleepResumeState = kUSB_HostRunGetBOSDescriptor5Done;
        }
        else
        {
            usb_echo("Can't not get Device BOS descriptor\r\n");
            mouseInstance->L1sleepResumeState = kStatus_Idle;
        }
    }
    else if (mouseInstance->L1sleepResumeState == kUSB_HostRunWaitGetBOSDescriptor) /* get the bos descriptor finish */
    {
        mouseInstance->L1sleepResumeState = kUSB_HostRunGetBOSDescriptorDone;
    }
    else
    {
    }
}

usb_status_t USB_HostParseBOSDescriptorLPMFeature(usb_device_handle deviceHandle, uint8_t *bosDescriptor)
{
    usb_descriptor_union_t *descriptorHead;
    usb_descriptor_union_t *descriptorTail;
    usb_descriptor_usb20_extension_t *usbExtension;
    usb_status_t usb_error;

    usb_error = kStatus_USB_Error;
    descriptorHead = (usb_descriptor_union_t *)bosDescriptor;
    descriptorTail = (usb_descriptor_union_t *)((uint8_t *)bosDescriptor +
                                                USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(
                                                    ((usb_descriptor_bos_t *)bosDescriptor)->wTotalLength) -
                                                1);
    if (1U == g_HostHidMouse.getBosretryDone)
    {
        if (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(((usb_descriptor_bos_t *)bosDescriptor)->wTotalLength) !=
            (g_HostHidMouse.mouseBosHeadBuffer[2] + (g_HostHidMouse.mouseBosHeadBuffer[3] << 8)))
        {
            return usb_error;
        }
        if (descriptorHead->common.bDescriptorType != USB_DESCRIPTOR_TYPE_BOS)
        {
            return usb_error;
        }
    }
    while (descriptorHead < descriptorTail)
    {
        if (descriptorHead->common.bDescriptorType == USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY)
        {
            if (descriptorHead->common.bData[0] == USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY_USB20_EXTENSION)
            {
                usbExtension = (usb_descriptor_usb20_extension_t *)descriptorHead;
                if (usbExtension->bmAttributes[0] & USB_DESCRIPTOR_DEVICE_CAPABILITY_USB20_EXTENSION_LPM_MASK)
                {
                    usb_error = kStatus_USB_Success;
                    break;
                }
            }
        }
        descriptorHead = (usb_descriptor_union_t *)((uint8_t *)descriptorHead + descriptorHead->common.bLength);
    }
    return usb_error;
}

usb_status_t USB_HostControlGetBOSDescriptor(usb_host_handle hostHandle,
                                             usb_device_handle deviceHandle,
                                             host_inner_transfer_callback_t callbackFn,
                                             void *callbackParam,
                                             void *buffer,
                                             uint32_t length)
{
    usb_host_transfer_t *transfer;
    uint32_t infoValue;

    if ((hostHandle == NULL) || (deviceHandle == NULL))
    {
        return kStatus_USB_InvalidHandle;
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Busy;
    }
    /* initialize transfer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = length;
    transfer->callbackFn = callbackFn;
    transfer->callbackParam = callbackParam;
    transfer->setupPacket->bmRequestType =
        USB_REQUEST_TYPE_RECIPIENT_DEVICE | USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_STANDARD;
    transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_GET_DESCRIPTOR;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN((uint16_t)(USB_DESCRIPTOR_TYPE_BOS << 8));
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(0x00U);
    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(length);
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &infoValue);

    if (USB_HostSendSetup(hostHandle, (usb_host_pipe_handle)infoValue, transfer) !=
        kStatus_USB_Success) /* call host driver api */
    {
#ifdef HOST_ECHO
        usb_echo("failed for USB_HostControlRemoteWakeup\r\n");
#endif
        USB_HostFreeTransfer(hostHandle, transfer);
        return kStatus_USB_Error;
    }
    return kStatus_USB_Success;
}

static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode)
{
    usb_status_t status = kStatus_USB_Success;

    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
            g_HostHidMouse.getBosretryDone = 0U;
            g_HostHidMouse.supportLPM = 0U;
            status = USB_HostHidMouseEvent(deviceHandle, configurationHandle, eventCode);
            break;

        case kUSB_HostEventNotSupported:
            usb_echo("device not supported.\r\n");
            break;

        case kUSB_HostEventEnumerationDone:
            status = USB_HostHidMouseEvent(deviceHandle, configurationHandle, eventCode);
            break;

        case kUSB_HostEventDetach:
            g_HostHidMouse.getBosretryDone = 0U;
            g_HostHidMouse.supportLPM = 0U;
            if (g_HostHidMouse.device_bos_descriptor)
            {
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
                SDK_Free(g_HostHidMouse.device_bos_descriptor);
#else
                USB_AppMemoryFree(g_HostHidMouse.device_bos_descriptor);
#endif
                g_HostHidMouse.device_bos_descriptor = NULL;
            }
            status = USB_HostHidMouseEvent(deviceHandle, configurationHandle, eventCode);
            break;
        case kUSB_HostEventL1SleepNotSupport:
            if (kStatus_Idle != g_HostHidMouse.L1sleepResumeState)
            {
                usb_echo("Device Don't Support LPM.\r\n");
            }
            g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
            break;
        case kUSB_HostEventL1SleepNYET:
            if (kStatus_Idle != g_HostHidMouse.L1sleepResumeState)
            {
                usb_echo("Device was unable to enter the L1 state at this time.\r\n");
            }
            break;

        case kUSB_HostEventL1Sleeped:
            if (kStatus_Idle != g_HostHidMouse.L1sleepResumeState)
            {
                USB_ControllerSuspended();
                g_HostHidMouse.L1sleepResumeState = kStatus_L1Sleeped;
            }
            else
            {
                g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
            }
            break;
        case kUSB_HostEventL1SleepError:
            if (kStatus_Idle != g_HostHidMouse.L1sleepResumeState)
            {
                usb_echo("Device failed to respond or an error occurred\r\n");
                g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
            }
            break;

        case kUSB_HostEventDetectResume:
            if (kStatus_Idle != g_HostHidMouse.L1sleepResumeState)
            {
                USB_WaitClockLocked();
            }
            break;
        case kUSB_HostEventL1Resumed:
            if (kStatus_Idle != g_HostHidMouse.L1sleepResumeState)
            {
                if (g_HostHidMouse.L1sleepBus)
                {
                    usb_echo("BUS has been resumed.\r\n");
                }
                else
                {
                    usb_echo("Device has been resumed.\r\n");
                }
            }
            g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
            break;
        default:
            break;
    }
    return status;
}

static void USB_HostApplicationInit(void)
{
    usb_status_t status = kStatus_USB_Success;

    USB_HostClockInit();

#if ((defined FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT))
    SYSMPU_Enable(SYSMPU, 0);
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

    status = USB_HostInit(CONTROLLER_ID, &g_HostHandle, USB_HostEvent);
    if (status != kStatus_USB_Success)
    {
        usb_echo("host init error\r\n");
        return;
    }
    USB_HostIsrEnable();

    usb_echo("host init done\r\n");
}


void USB_PowerPreSwitchHook(void)
{
    HW_TimerControl(0U);

    DbgConsole_Deinit();

    BOARD_DeinitPins();

    USB_PreLowpowerMode();
}

void USB_PowerPostSwitchHook(void)
{
    USB_WaitClockLocked();
    USB_PostLowpowerMode();
    BOARD_InitPins();
    BOARD_InitDebugConsole();
    HW_TimerControl(1U);
}

void USB_HostL1SleepResumeTask(void)
{
    uint8_t command;

    if (kStatus_USB_Success != DbgConsole_TryGetchar((char *)&command))
    {
        command = 0;
    }
    switch (g_HostHidMouse.L1sleepResumeState)
    {
        case kStatus_Idle:
            if ('s' == command)
            {
                if (g_HostHidMouse.deviceState == kStatus_DEV_Attached)
                {
                    g_HostHidMouse.L1sleepResumeState = kStatus_L1Sleepding;
                    usb_echo("Start suspend USB BUS...\r\n");
                }
                else
                {
                    usb_echo("Device is not attached\r\n");
                }
            }
            else
            {
                if (command)
                {
                    usb_echo("Please Enter 's' to start suspend test\r\n");
                }
            }
            break;
        case kStatus_L1Sleepding:
            if (g_HostHidMouse.deviceSupportRemoteWakeup)
            {
                usb_echo(
                    "\r\nPlease Enter: \r\n\t1. Enable remote wakeup feature.\r\n\t2. Disable remote wakeup "
                    "feature.\r\n");
                g_HostHidMouse.L1sleepResumeState = kStatus_SuspendSetRemoteWakeup;
            }
            else
            {
                g_HostHidMouse.L1sleepResumeState = kUSB_HostRunStartGetBOSDescriptor5;
            }
            break;

        case kStatus_SuspendSetRemoteWakeup:
            if ('1' == command)
            {
                usb_echo("1");
                g_HostHidMouse.L1SetRemoteWakeup = 1U;
                g_HostHidMouse.L1sleepResumeState = kUSB_HostRunStartGetBOSDescriptor5;
            }
            else if ('2' == command)
            {
                usb_echo("2");
                g_HostHidMouse.L1SetRemoteWakeup = 0U;
                g_HostHidMouse.L1sleepResumeState = kUSB_HostRunStartGetBOSDescriptor5;
            }
            else
            {
            }
            break;
        case kUSB_HostRunStartGetBOSDescriptor5:
            if (1U == g_HostHidMouse.getBosretryDone)
            {
                g_HostHidMouse.L1sleepResumeState = kUSB_HostRunGetBOSDescriptorDone;
            }
            else
            {
                g_HostHidMouse.L1sleepResumeState = kUSB_HostRunGetBOSDescriptor5;
            }
            break;
        case kUSB_HostRunGetBOSDescriptor5:
            g_HostHidMouse.L1sleepResumeState = kUSB_HostRunWaitGetBOSDescriptor5;
            if (kStatus_USB_Success != USB_HostControlGetBOSDescriptor(g_HostHandle, g_HostHidMouse.deviceHandle,
                                                                       USB_HostHidControlGetBOSCallback, g_HostHandle,
                                                                       (void *)&g_HostHidMouse.mouseBosHeadBuffer[0],
                                                                       USB_DESCRIPTOR_LENGTH_BOS_DESCRIPTOR))
            {
                g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
                usb_echo("error in get bos descriptor\r\n");
            }
            break;
        case kUSB_HostRunWaitGetBOSDescriptor5:
            break;
        case kUSB_HostRunGetBOSDescriptor5Done:
            g_HostHidMouse.L1sleepResumeState = kUSB_HostRunWaitGetBOSDescriptor;
            uint32_t bosLenght;
            bosLenght = g_HostHidMouse.mouseBosHeadBuffer[2] + (g_HostHidMouse.mouseBosHeadBuffer[3] << 8);
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
            g_HostHidMouse.device_bos_descriptor =  uint8_t *)SDK_Malloc((bosLenght & 0xFFFFFFFCu) + 4, USB_CACHE_LINESIZE);
#else
            g_HostHidMouse.device_bos_descriptor = USB_AppMemoryAllocate(bosLenght);
#endif
            if (NULL == g_HostHidMouse.device_bos_descriptor)
            {
                usb_echo("Error in malloc\r\n");
                g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
            }
            else
            {
                if (kStatus_USB_Success !=
                    USB_HostControlGetBOSDescriptor(g_HostHandle, g_HostHidMouse.deviceHandle,
                                                    USB_HostHidControlGetBOSCallback, g_HostHandle,
                                                    g_HostHidMouse.device_bos_descriptor, bosLenght))
                {
                    USB_OsaMemoryFree(g_HostHidMouse.device_bos_descriptor);
                    g_HostHidMouse.device_bos_descriptor = NULL;
                    usb_echo("error in get bos descriptor\r\n");
                }
            }
            break;
        case kUSB_HostRunWaitGetBOSDescriptor:
            break;
        case kUSB_HostRunGetBOSDescriptorDone:
            g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
            g_HostHidMouse.getBosretryDone = 1;
            if (1U == g_HostHidMouse.supportLPM)
            {
                g_HostHidMouse.L1sleepResumeState = kStatus_L1StartSleep;
            }
            else
            {
                if (kStatus_USB_Success != USB_HostParseBOSDescriptorLPMFeature(g_HostHidMouse.deviceHandle,
                                                                                g_HostHidMouse.device_bos_descriptor))
                {
                    usb_echo("Device don't support Linker Power Managent(LPM)\r\n");
                    g_HostHidMouse.getBosretryDone = 0;
                }
                else
                {
                    g_HostHidMouse.L1sleepResumeState = kStatus_L1StartSleep;
                    g_HostHidMouse.supportLPM = 1U;
                }
            }
            DbgConsole_Flush();
            break;
        case kStatus_L1StartSleep:
            g_HostHidMouse.L1sleepBus = 0;
            g_HostHidMouse.L1sleepResumeState = kStatus_L1SleepRequest;
            uint8_t hirdValue;
            uint8_t lpmParam;
            /*hird value should be 0~15.please refer to usb LPM spec*/
            hirdValue = LPM_HIRD_VALUE;
            lpmParam = (uint8_t)(hirdValue | (g_HostHidMouse.L1SetRemoteWakeup << 7));
            USB_HostL1SleepDeviceResquestConfig(g_HostHandle, &lpmParam);

            if (kStatus_USB_Success ==
                USB_HostL1SleepDeviceResquest(g_HostHandle, g_HostHidMouse.deviceHandle, g_HostHidMouse.L1sleepBus))
            {
            }
            else
            {
                usb_echo("Send L1 sleep request failed.\r\n");
                g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
            }
            DbgConsole_Flush();
            break;
        case kStatus_L1SleepRequest:
            break;
        case kStatus_L1Sleeped:
            DbgConsole_Flush();
            if (g_HostHidMouse.L1sleepBus)
            {
                usb_echo("BUS has been suspended.\r\n");
            }
            else
            {
                usb_echo("Device has been suspended.\r\n");
            }
            DbgConsole_Flush();
            usb_echo("Please Press wakeup switch(%s) to start resume test.\r\n", SW_GetName());
            if (g_HostHidMouse.L1SetRemoteWakeup)
            {
                usb_echo("Or, wait for device sends resume signal.\r\n");
            }
            DbgConsole_Flush();
            USB_PowerPreSwitchHook();
            SW_IntControl(1);
            g_HostHidMouse.L1sleepResumeState = kStatus_L1WaitResume;
            if (kStatus_Success != USB_EnterLowpowerMode())
            {
                g_HostHidMouse.selfWakeup = 1U;
                USB_PowerPostSwitchHook();
                usb_echo("Enter VLPS mode failed!\r\n");
            }
            else
            {
                USB_PowerPostSwitchHook();
            }
            break;
        case kStatus_L1WaitResume:
            if (g_HostHidMouse.selfWakeup)
            {
                g_HostHidMouse.selfWakeup = 0U;
                usb_echo("Start L1 resume the device.\r\n");
                g_HostHidMouse.L1sleepResumeState = kStatus_L1ResumeRequest;
                if (kStatus_USB_Success == USB_HostL1ResumeDeviceResquest(g_HostHandle, g_HostHidMouse.deviceHandle,
                                                                          g_HostHidMouse.L1sleepBus))
                {
                }
                else
                {
                    g_HostHidMouse.L1sleepResumeState = kStatus_Idle;
                    usb_echo("Send resume signal failed.\r\n");
                }
            }
            break;
        case kStatus_L1ResumeRequest:
            break;
        default:
            break;
    }
    command = 0;
}

int main(void)
{
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitPins();
    BOARD_BootClockFROHF96M();
    BOARD_InitDebugConsole();

    NVIC_ClearPendingIRQ(USB0_IRQn);
    NVIC_ClearPendingIRQ(USB0_NEEDCLK_IRQn);
    NVIC_ClearPendingIRQ(USB1_IRQn);
    NVIC_ClearPendingIRQ(USB1_NEEDCLK_IRQn);

    POWER_DisablePD(kPDRUNCFG_PD_USB0_PHY); /*< Turn on USB0 Phy */
    POWER_DisablePD(kPDRUNCFG_PD_USB1_PHY); /*< Turn on USB1 Phy */

    /* reset the IP to make sure it's in reset state. */
    RESET_PeripheralReset(kUSB0D_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB0HSL_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB0HMR_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1H_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1D_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1RAM_RST_SHIFT_RSTn);

#if (defined USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS)
    POWER_DisablePD(kPDRUNCFG_PD_USB1_PHY); /*< Turn on USB Phy */
    /* enable USB IP clock */
    CLOCK_EnableUsbhs0HostClock(kCLOCK_UsbSrcUnused, 0U);
    /* enable usb1 host clock */
    CLOCK_EnableClock(kCLOCK_Usbh1);
    /* enable usb1 device clock */
    CLOCK_EnableClock(kCLOCK_Usbd1);
    *((uint32_t *)(USBHSH_BASE + 0x50)) &= ~USBHSH_PORTMODE_DEV_ENABLE_MASK;
    *((uint32_t *)(USBHSH_BASE + 0x50)) &= ~USBHSH_PORTMODE_SW_CTRL_PDCOM_MASK;
    /* disable usb1 device clock */
    CLOCK_DisableClock(kCLOCK_Usbd1);
    /* disable usb1 host clock */
    CLOCK_DisableClock(kCLOCK_Usbh1);
#endif
#if (defined USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI)
    POWER_DisablePD(kPDRUNCFG_PD_USB0_PHY); /*< Turn on USB Phy */
    CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk, 1, false);
    CLOCK_AttachClk(kFRO_HF_to_USB0_CLK);
    /* enable usb0 host clock */
    CLOCK_EnableClock(kCLOCK_Usbhmr0);
    CLOCK_EnableClock(kCLOCK_Usbhsl0);
    /* enable usb0 device clock */
    CLOCK_EnableClock(kCLOCK_Usbd0);
    *((uint32_t *)(USBFSH_BASE + 0x5C)) &= ~USBFSH_PORTMODE_DEV_ENABLE_MASK;

    /* disable usb0 device clock */
    CLOCK_DisableClock(kCLOCK_Usbd0);
    /* disable usb0 host clock */
    CLOCK_DisableClock(kCLOCK_Usbhsl0);
    CLOCK_DisableClock(kCLOCK_Usbhmr0);
#endif
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    USB_LowpowerModeInit();
#endif

    USB_HostApplicationInit();

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    HW_TimerControl(1);
    usb_echo("Please Enter 's' to start L1 suspend test\r\n");
#endif

    while (1)
    {
        USB_HostTaskFn(g_HostHandle);
        USB_HostHidMouseTask(&g_HostHidMouse);

        USB_HostL1SleepResumeTask();
    }
}
