/*
    Copyright (C) 2016 Jonathan Struebel
    Modifications copyright (C) 2020 Alex Lewontin

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    USB_HID.h
 * @brief   USB driver config header.
 *
 * @addtogroup USB
 * @{
 */

#ifndef USB_HID_H
#define USB_HID_H

#include "hal_usb_lld.h"

extern const USBConfig usb_hid;
extern const USBHIDConfig usbhidcfg;
extern USBHIDDriver UHD;

typedef struct {
	#ifdef USB_HID_MSG_DATA
	USB_HID_DATA
	#else
    uint8_t                    frameId;
	uint8_t					   volume;
    uint8_t                    buttons;
    uint8_t                    buzzer;
	#endif
}MyUSBidReport_TypeDef;


#define USB_DESC_STRING(...)                                                                       \
  {                                                                                                \
    USB_DESC_BYTE((sizeof((int[]){__VA_ARGS__}) / sizeof(int)) + 2),                               \
        USB_DESC_BYTE(USB_DESCRIPTOR_STRING), __VA_ARGS__                                          \
  }

#ifdef __cplusplus
extern "C" {
#endif
msg_t hidGetReport(MyUSBidReport_TypeDef *pHidReport);
msg_t hidSetReport(MyUSBidReport_TypeDef *pHidReport);
void usbHidProcessReceivedMsg(MyUSBidReport_TypeDef  *pHidReport, char *eventSource);

#ifdef __cplusplus
}
#endif

#endif  /* USB_HID_H */

/** @} */
