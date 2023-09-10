/**
  *
  * Licensed to the Apache Software Foundation (ASF) under one or more
  * contributor license agreements.  See the NOTICE file distributed with
  * this work for additional information regarding copyright ownership.  The
  * ASF licenses this file to you under the Apache License, Version 2.0 (the
  * "License"); you may not use this file except in compliance with the
  * License.  You may obtain a copy of the License at
  *
  *   http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
  * License for the specific language governing permissions and limitations
  * under the License.
  *
  */


#ifndef WBVAL
#define WBVAL(x) (unsigned char)((x) & 0xFF), (unsigned char)(((x) >> 8) & 0xFF)
#endif




/*!< USBD CONFIG */
#define USBD_VERSION 0x0110
#define USBD_PRODUCT_VERSION 0x0001
#define USBD_VID 0xffff
#define USBD_PID 0xffff
#define USBD_MAX_POWER 0xfa
#define USBD_LANGID_STRING 1033
#define USBD_CONFIG_DESCRIPTOR_SIZE 32




/*!< USBD ENDPOINT CONFIG */

#define USBD_IF0_AL0_EP0_ADDR 0x01
#define USBD_IF0_AL0_EP0_SIZE 0x40
#define USBD_IF0_AL0_EP0_INTERVAL 0x00

#define USBD_IF0_AL0_EP1_ADDR 0x82
#define USBD_IF0_AL0_EP1_SIZE 0x40
#define USBD_IF0_AL0_EP1_INTERVAL 0x00




/*!< USBD HID CONFIG */




/*!< USBD Descriptor */
const unsigned char usbd_descriptor[] = {   
/********************************************** Device Descriptor */
    0x12,                                       /*!< bLength */
    0x01,                                       /*!< bDescriptorType */
    WBVAL(USBD_VERSION),                        /*!< bcdUSB */
    0x00,                                       /*!< bDeviceClass */
    0x00,                                       /*!< bDeviceSubClass */
    0x00,                                       /*!< bDeviceProtocol */
    0x40,                                       /*!< bMaxPacketSize */
    WBVAL(USBD_VID),                            /*!< idVendor */
    WBVAL(USBD_PID),                            /*!< idProduct */
    WBVAL(USBD_PRODUCT_VERSION),                /*!< bcdDevice */
    0x01,                                       /*!< iManufacturer */
    0x02,                                       /*!< iProduct */
    0x03,                                       /*!< iSerial */
    0x01,                                       /*!< bNumConfigurations */
/********************************************** Config Descriptor */
    0x09,                                       /*!< bLength */
    0x02,                                       /*!< bDescriptorType */
    WBVAL(USBD_CONFIG_DESCRIPTOR_SIZE),         /*!< wTotalLength */
    0x01,                                       /*!< bNumInterfaces */
    0x01,                                       /*!< bConfigurationValue */
    0x00,                                       /*!< iConfiguration */
    0xa0,                                       /*!< bmAttributes */
    USBD_MAX_POWER,                             /*!< bMaxPower */
/********************************************** Interface 0 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x00,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x02,                                       /*!< bNumEndpoints */
    0x08,                                       /*!< bInterfaceClass */
    0x06,                                       /*!< bInterfaceSubClass */
    0x50,                                       /*!< bInterfaceProtocol */
    0x00,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of MSC */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF0_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x02,                                       /*!< bmAttributes */
    WBVAL(USBD_IF0_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF0_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Endpoint 1 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF0_AL0_EP1_ADDR,                      /*!< bEndpointAddress */
    0x02,                                       /*!< bmAttributes */
    WBVAL(USBD_IF0_AL0_EP1_SIZE),               /*!< wMaxPacketSize */
    USBD_IF0_AL0_EP1_INTERVAL,                  /*!< bInterval */
/********************************************** Language ID String Descriptor */
    0x04,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    WBVAL(USBD_LANGID_STRING),                  /*!< wLangID0 */
/********************************************** String 1 Descriptor */
/* Your Manufacturer */
    0x24,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x59, 0x00,                                 /*!< 'Y' wcChar0 */
    0x6f, 0x00,                                 /*!< 'o' wcChar1 */
    0x75, 0x00,                                 /*!< 'u' wcChar2 */
    0x72, 0x00,                                 /*!< 'r' wcChar3 */
    0x20, 0x00,                                 /*!< ' ' wcChar4 */
    0x4d, 0x00,                                 /*!< 'M' wcChar5 */
    0x61, 0x00,                                 /*!< 'a' wcChar6 */
    0x6e, 0x00,                                 /*!< 'n' wcChar7 */
    0x75, 0x00,                                 /*!< 'u' wcChar8 */
    0x66, 0x00,                                 /*!< 'f' wcChar9 */
    0x61, 0x00,                                 /*!< 'a' wcChar10 */
    0x63, 0x00,                                 /*!< 'c' wcChar11 */
    0x74, 0x00,                                 /*!< 't' wcChar12 */
    0x75, 0x00,                                 /*!< 'u' wcChar13 */
    0x72, 0x00,                                 /*!< 'r' wcChar14 */
    0x65, 0x00,                                 /*!< 'e' wcChar15 */
    0x72, 0x00,                                 /*!< 'r' wcChar16 */
/********************************************** String 2 Descriptor */
/* Your Product */
    0x1a,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x59, 0x00,                                 /*!< 'Y' wcChar0 */
    0x6f, 0x00,                                 /*!< 'o' wcChar1 */
    0x75, 0x00,                                 /*!< 'u' wcChar2 */
    0x72, 0x00,                                 /*!< 'r' wcChar3 */
    0x20, 0x00,                                 /*!< ' ' wcChar4 */
    0x50, 0x00,                                 /*!< 'P' wcChar5 */
    0x72, 0x00,                                 /*!< 'r' wcChar6 */
    0x6f, 0x00,                                 /*!< 'o' wcChar7 */
    0x64, 0x00,                                 /*!< 'd' wcChar8 */
    0x75, 0x00,                                 /*!< 'u' wcChar9 */
    0x63, 0x00,                                 /*!< 'c' wcChar10 */
    0x74, 0x00,                                 /*!< 't' wcChar11 */
/********************************************** String 3 Descriptor */
/* Your Serial Number */
    0x26,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x59, 0x00,                                 /*!< 'Y' wcChar0 */
    0x6f, 0x00,                                 /*!< 'o' wcChar1 */
    0x75, 0x00,                                 /*!< 'u' wcChar2 */
    0x72, 0x00,                                 /*!< 'r' wcChar3 */
    0x20, 0x00,                                 /*!< ' ' wcChar4 */
    0x53, 0x00,                                 /*!< 'S' wcChar5 */
    0x65, 0x00,                                 /*!< 'e' wcChar6 */
    0x72, 0x00,                                 /*!< 'r' wcChar7 */
    0x69, 0x00,                                 /*!< 'i' wcChar8 */
    0x61, 0x00,                                 /*!< 'a' wcChar9 */
    0x6c, 0x00,                                 /*!< 'l' wcChar10 */
    0x20, 0x00,                                 /*!< ' ' wcChar11 */
    0x4e, 0x00,                                 /*!< 'N' wcChar12 */
    0x75, 0x00,                                 /*!< 'u' wcChar13 */
    0x6d, 0x00,                                 /*!< 'm' wcChar14 */
    0x62, 0x00,                                 /*!< 'b' wcChar15 */
    0x65, 0x00,                                 /*!< 'e' wcChar16 */
    0x72, 0x00,                                 /*!< 'r' wcChar17 */
    0x00
};



/*******************************************************END OF FILE*************/
