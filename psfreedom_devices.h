/*
 * psfreedom_devices.h -- PS3 Jailbreak exploit Gadget Driver
 *
 * Copyright (C) Youness Alaoui (KaKaRoTo)
 *
 * This software is distributed under the terms of the GNU General Public
 * License ("GPL") version 3, as published by the Free Software Foundation.
 *
 * This code is based in part on:
 * PSGroove
 *
 */

#include "pl3/shellcode_egghunt.h"
#include "pl3/default_payload_3_41.h"
#include "pl3/default_payload_3_01.h"
#include "pl3/dump_lv2.h"


#define MAGIC_NUMBER		0x50, 0x53, 0x46, 0x72, 0x65, 0x65, 0x64, 0x6d

#if defined (FIRMWARE_3_41)
#define RTOC_TABLE		0x80, 0x00, 0x00, 0x00, 0x00, 0x33, 0xe7, 0x20
#elif defined (FIRMWARE_3_15)
#define RTOC_TABLE		0x80, 0x00, 0x00, 0x00, 0x00, 0x33, 0xda, 0x10
#elif defined (FIRMWARE_3_01)
#define RTOC_TABLE		0x80, 0x00, 0x00, 0x00, 0x00, 0x32, 0x06, 0x40
#else
#error "You must specify the target firmware." \
  " define a supported FIRMWARE_X_YZ in config.h and recompile."
#endif /* FIRMWARE_X_YZ */


#ifdef USE_JIG
#define default_shellcode shellcode_egghunt

#if defined (FIRMWARE_3_41)
#define default_payload default_payload_3_41
#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xee
#define SHELLCODE_ADDR_LOW	0x70
#elif defined (FIRMWARE_3_15)
#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xde
#define SHELLCODE_ADDR_LOW	0x30
#elif defined (FIRMWARE_3_01)
#define default_payload default_payload_3_01
#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xFB
#define SHELLCODE_ADDR_LOW	0xC8
#endif /* FIRMWARE_X_YZ */

#define SHELLCODE_PAGE		0x80, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00
#define SHELLCODE_DESTINATION	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW
#define SHELLCODE_PTR 		SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x08
#define SHELLCODE_ADDRESS	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x18

#define PORT1_NUM_CONFIGS	4

#else /* USE_JIG */

#define default_shellcode shellcode_egghunt
#define default_payload dump_lv2

#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x00
//#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x00
//#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00
#define SHELLCODE_ADDR_LOW	0x00

#define SHELLCODE_PAGE		SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW
#define SHELLCODE_DESTINATION	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x20
#define SHELLCODE_PTR 		SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x28
#define SHELLCODE_ADDRESS	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x38

#define PORT1_NUM_CONFIGS	100

#endif /* USE_JIG */

/* Hub endpoint Descriptor */
static struct usb_endpoint_descriptor jig_out_endpoint_desc = {
  .bLength =		USB_DT_ENDPOINT_SIZE,
  .bDescriptorType =	USB_DT_ENDPOINT,
  .bEndpointAddress =	USB_DIR_OUT | 0x02,
  .bmAttributes =	USB_ENDPOINT_XFER_BULK,
  .wMaxPacketSize =	__constant_cpu_to_le16(8),
  .bInterval =		0x00,
};

/* Hub endpoint Descriptor */
static struct usb_endpoint_descriptor jig_in_endpoint_desc = {
  .bLength =		USB_DT_ENDPOINT_SIZE,
  .bDescriptorType =	USB_DT_ENDPOINT,
  .bEndpointAddress =	USB_DIR_IN | 0x01,
  .bmAttributes =	USB_ENDPOINT_XFER_BULK,
  .wMaxPacketSize =	__constant_cpu_to_le16(8),
  .bInterval =		0x00,
};

static u8 jig_response[64] = {
  SHELLCODE_PTR,
  SHELLCODE_ADDRESS,
  RTOC_TABLE
};

static u8 port1_device_desc[] = {
  0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
  0xAA, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
  0x00, PORT1_NUM_CONFIGS,
};

static u8 port1_short_config_desc[] = {
  0x09, 0x02, 0x00, 0x0f, 0x01, 0x00, 0x00, 0x80,
};

static u8 port1_config_desc_prefix[] = {
  0x09, 0x02, 0x12, 0x00, 0x01, 0x00, 0x00, 0x80, 0xfa,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  MAGIC_NUMBER,
#ifndef USE_JIG
  SHELLCODE_PTR,
  SHELLCODE_ADDRESS,
  RTOC_TABLE
#endif
};

static u8 port2_device_desc[] = {
  0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
  0xAA, 0xAA, 0xBB, 0xBB, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01,
};

static u8 port2_config_desc[] = {
  // config
  0x09, 0x02, 0x16, 0x00, 0x01, 0x01, 0x00, 0x80, 0x01,
  // interface
  0x09, 0x04, 0x00, 0x00, 0x00, 0xFE, 0x01, 0x02, 0x00,
  // extra
  0x04, 0x21, 0xb4, 0x2f,
};

static u8 port3_device_desc[] = {
  0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
  0xAA, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02,
};


const u8 port0_device_desc[] = {
  0x50, 0x53, 0x46, 0x72, 0x65, 0x65, 0x64, 0x6f,
  0x6d, 0x20, 0x62, 0x79, 0x20, 0x4b, 0x61, 0x4b,
  0x61, 0x52, 0x6f, 0x54, 0x6f, 0x0a, 0x52, 0x65,
  0x6c, 0x65, 0x61, 0x73, 0x65, 0x64, 0x20, 0x75,
  0x6e, 0x64, 0x65, 0x72, 0x20, 0x47, 0x50, 0x4c,
  0x20, 0x76, 0x33, 0x0a, 0x00, 0x00, 0x00, 0x00,
};

static u8 port3_config_desc[] = {
  0x09, 0x02, 0x4d, 0x0a, 0x01, 0x01, 0x00, 0x80, 0x01,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
};

static u8 port4_device_desc[] = {
  0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
  0xAA, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03,
};

static u8 port4_config_desc_1[] = {
  // config
  0x09, 0x02, 0x12, 0x00, 0x01, 0x01, 0x00, 0x80,
  0x01,
  // interface
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
  0x00,
};

static u8 port4_short_config_desc_2[] = {
  // config
  0x09, 0x02, 0x12, 0x00, 0x01, 0x01, 0x00, 0x80,
};

static u8 port4_config_desc_2[] = {
  // config
  0x09, 0x02, 0x00, 0x00, 0x01, 0x01, 0x00, 0x80, 0x01,
  // interface
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
};

static u8 port4_config_desc_3[] = {
  0x09, 0x02, 0x30, 0x00, 0x01, 0x01, 0x00, 0x80, 0x01,
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
  0x3e, 0x21, 0x00, 0x00, 0x00, 0x00,
  MAGIC_NUMBER, /* magic number to look for in the start of the page */
  SHELLCODE_PAGE, /* Initial data search ptr */
  SHELLCODE_DESTINATION, /* destination ptr for heap structure (jig response) */
};

static u8 port5_device_desc[] = {
  0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
  0x4c, 0x05, 0xeb, 0x02, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01,
};

static u8 port5_config_desc[] = {
  // config
  0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80, 0x01,
  // interface
  0x09, 0x04, 0x01, 0x00, 0x02, 0xff, 0x00, 0x00, 0x00,
  // endpoint
  0x07, 0x05, 0x02, 0x02, 0x08, 0x00, 0x00,
  // endpoint
  0x07, 0x05, 0x81, 0x02, 0x08, 0x00, 0x00,
};

static u8 port6_device_desc[] = {
  0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
  0xaa, 0xaa, 0xc0, 0xde, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01,
};

static u8 port6_config_desc[] = {
  // config
  0x09, 0x02, 0x12, 0x00, 0x01, 0x01, 0x00, 0x80, 0x01,
  // interface
  0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
};


