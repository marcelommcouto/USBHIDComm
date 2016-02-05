/*
===============================================================================
 Name        : USBHIDComm.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>

#include "descriptor.h"
#include "USB.h"

/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevHIDReportBuffer[GENERIC_REPORT_SIZE];

/** LPCUSBlib HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Generic_HID_Interface =
{
	.Config =
	{
		.InterfaceNumber              = 0,

		.ReportINEndpointNumber       = GENERIC_IN_EPNUM,
		.ReportINEndpointSize         = GENERIC_EPSIZE,
		.ReportINEndpointDoubleBank   = false,

		.PrevReportINBuffer           = PrevHIDReportBuffer,
		.PrevReportINBufferSize       = sizeof(PrevHIDReportBuffer),
	},
};

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
void EVENT_USB_Device_StartOfFrame(void);

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize);
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize);

int main(void)
{
	SystemInit();

	SystemCoreClockUpdate();

	USB_CurrentMode = USB_MODE_Device;
	USB_Init();

	// Initialize ports...
	LPC_GPIO0->FIODIR |= 0xff0;

    // Enter an infinite loop, just incrementing a counter
    while(1)
    {
    	HID_Device_USBTask(&Generic_HID_Interface);
    	USB_USBTask();
    }
    return 0 ;
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
//	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
//	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Generic_HID_Interface);

	USB_Device_EnableSOFEvents();

//	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Generic_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Generic_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent
 *
 *  \return Boolean true to force the sending of the report, false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	uint8_t* Data = (uint8_t*)ReportData;
//	uint8_t JoyStatus_LCL    = Joystick_GetStatus();
//	uint8_t ButtonStatus_LCL = Buttons_GetStatus();
	uint8_t ret = 0;

//	if (JoyStatus_LCL & JOY_UP) 			ret |= 0x01;
//	if (JoyStatus_LCL & JOY_LEFT)     		ret |= 0x02;
//	if (JoyStatus_LCL & JOY_RIGHT)			ret |= 0x04;
//	if (JoyStatus_LCL & JOY_PRESS)			ret |= 0x08;
//	if (JoyStatus_LCL & JOY_DOWN)			ret |= 0x10;
//	if (ButtonStatus_LCL & BUTTONS_BUTTON1)	ret |= 0x20;

	Data[0] = ret;

	*ReportSize = GENERIC_REPORT_SIZE;
	return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	uint8_t* Data = (uint8_t*)ReportData;
	LPC_GPIO0->FIOSET |= (0xff << 4);
//
	if (Data[0] & 0x01) LPC_GPIO0->FIOCLR |= (1 << 4);
	if (Data[0] & 0x02) LPC_GPIO0->FIOCLR |= (1 << 5);
	if (Data[0] & 0x04) LPC_GPIO0->FIOCLR |= (1 << 6);
	if (Data[0] & 0x08) LPC_GPIO0->FIOCLR |= (1 << 7);
	if (Data[0] & 0x10) LPC_GPIO0->FIOCLR |= (1 << 8);
	if (Data[0] & 0x20) LPC_GPIO0->FIOCLR |= (1 << 9);
	if (Data[0] & 0x40) LPC_GPIO0->FIOCLR |= (1 << 10);
	if (Data[0] & 0x80) LPC_GPIO0->FIOCLR |= (1 << 11);

//	LEDs_SetAllLEDs(NewLEDMask);
}
