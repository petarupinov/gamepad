/* USB Keyboard Example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 * Copyright (c) 2009 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "usb_gamepad.h"

#define STR_MANUFACTURER        L"P.UPINOV"
#define STR_PRODUCT             L"P.UPINOV AVR USB GAMEPAD"

#define VENDOR_ID               0x0001  //0x16C0
#define PRODUCT_ID              0x0002  //0x047C


/*
===========================================================================
=   Endpoint Buffer Configuration                                         =
===========================================================================
*/

#define ENDPOINT0_SIZE          32

#define GAMEPAD_INTERFACE       0
#define GAMEPAD_ENDPOINT        3
#define GAMEPAD_SIZE            8
#define GAMEPAD_BUFFER          EP_DOUBLE_BUFFER

static const uint8_t PROGMEM endpoint_config_table[] = {
  0,
  0,
  1, EP_TYPE_INTERRUPT_IN, EP_SIZE(GAMEPAD_SIZE) | GAMEPAD_BUFFER,
  0
};

/*
===========================================================================
=   DEVICE DESCRIPTOR                                                     =
===========================================================================
*/

static uint8_t PROGMEM device_descriptor[] = {
  18,                                // bLength
  1,                                 // bDescriptorType
  0x00, 0x02,                        // bcdUSB
  0,                                 // bDeviceClass
  0,                                 // bDeviceSubClass
  0,                                 // bDeviceProtocol
  ENDPOINT0_SIZE,                    // bMaxPacketSize0
  LSB(VENDOR_ID), MSB(VENDOR_ID),    // idVendor
  LSB(PRODUCT_ID), MSB(PRODUCT_ID),  // idProduct
  0x00, 0x01,                        // bcdDevice
  1,                                 // iManufacturer
  2,                                 // iProduct
  3,                                 // iSerialNumber
  1                                  // bNumConfigurations
};

/*
===========================================================================
=   REPORT DESCRIPTOR                                                     =
===========================================================================
*/

static uint8_t PROGMEM gamepad_hid_report_desc[] = { 
  0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
  0x09, 0x05,        // USAGE (Gamepad)
  0xA1, 0x01,        // COLLECTION (Application)
  0x09, 0x01,        //   USAGE (Pointer)
  0xA1, 0x00,        //   COLLECTION (Physical)
  0x09, 0x30,        //     USAGE (X)
  0x09, 0x31,        //     USAGE (Y)
  0x15, 0x00,        //     LOGICAL_MINIMUM (0)
  0x26, 0xFF, 0x00,  //     LOGICAL_MAXIMUM (255)
  0x75, 0x08,        //     REPORT_SIZE (8)
  0x95, 0x02,        //     REPORT_COUNT (2)
  0x81, 0x02,        //     INPUT (Data,Var,Abs)
  0xC0,              //   END_COLLECTION
  0x05, 0x09,        //   USAGE_PAGE (Button)
  0x19, 0x01,        //   USAGE_MINIMUM (Button 1)
  0x29, 0x02,        //   USAGE_MAXIMUM (Button 2)
  0x15, 0x00,        //   LOGICAL_MINIMUM (0)
  0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
  0x75, 0x01,        //   REPORT_SIZE (1)
  0x95, 0x02,        //   REPORT_COUNT (2)
  0x81, 0x02,        //   INPUT (Data,Var,Abs)
    0x95, 0x06,        //   REPORT_COUNT (6)
    0x81, 0x03,        //   INPUT (Constant,Var,Abs)
  0xC0               // END_COLLECTION
};

/*
===========================================================================
=   CONFIGURATION 1 DESCRIPTOR (9+9+9+7)                                  =
=       CONFIGURATION   DESCRIPTOR - 9                                    =
=       INTERFACE       DESCRIPTOR - 9                                    =
=       ENDPOINT        DESCRIPTOR - 9                                    =
===========================================================================
*/

#define CONFIG1_DESC_SIZE       (9+9+9+7)
#define GAMEPAD_HID_DESC_OFFSET (9+9)
static uint8_t PROGMEM config1_descriptor[CONFIG1_DESC_SIZE] = {
  // configuration descriptor, USB rev2.0 spec. glava 9.6.3, page 264-266, table 9-10
  9,                                    // bLength;
  2,                                    // bDescriptorType;
  LSB(CONFIG1_DESC_SIZE),               // wTotalLength
  MSB(CONFIG1_DESC_SIZE),
  1,                                    // bNumInterfaces
  1,                                    // bConfigurationValue
  0,                                    // iConfiguration
  0x80,                                 // bmAttributes
  50,                                   // bMaxPower
  // interface descriptor, USB rev2.0 spec. glava 9.6.5, page 267-269, table 9-12
  9,                                    // bLength
  4,                                    // bDescriptorType
  GAMEPAD_INTERFACE,                    // bInterfaceNumber
  0,                                    // bAlternateSetting
  1,                                    // bNumEndpoints
  0x03,                                 // bInterfaceClass (0x03 = HID)
  0x00,                                 // bInterfaceSubClass (0x00 = No Boot)
  0x00,                                 // bInterfaceProtocol (0x00 = No Protocol)
  0,                                    // iInterface
  // HID interface descriptor, HID 1.11 spec, 6.2.1
  9,                                    // bLength
  0x21,                                 // bDescriptorType
  0x11, 0x01,                           // bcdHID
  0,                                    // bCountryCode
  1,                                    // bNumDescriptors
  0x22,                                 // bDescriptorType
  sizeof(gamepad_hid_report_desc),      // wDescriptorLength
  0,
  // endpoint descriptor, USB rev2.0 spec. 9.6.6, page 269-271, table 9-13
  7,                                    // bLength
  5,                                    // bDescriptorType
  GAMEPAD_ENDPOINT | 0x80,              // bEndpointAddress
  0x03,                                 // bmAttributes (0x03=intr)
  GAMEPAD_SIZE, 0,                      // wMaxPacketSize
  10                                    // bInterval
};

/*
===========================================================================
=   STRING DESCRIPTOR                                                     =
===========================================================================
*/

struct usb_string_descriptor_struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  int16_t wString[];
};

static struct usb_string_descriptor_struct PROGMEM string0 = {
  4,
  3,
  {0x0409}
};

static struct usb_string_descriptor_struct PROGMEM string1 = {
  sizeof(STR_MANUFACTURER),
  3,
  STR_MANUFACTURER
};

static struct usb_string_descriptor_struct PROGMEM string2 = {
  sizeof(STR_PRODUCT),
  3,
  STR_PRODUCT
};


// Which of the descriptors will be send
// for special request to host (wValue, wIndex).
static struct descriptor_list_struct {
  uint16_t      wValue;
  uint16_t      wIndex;
  const uint8_t *addr;
  uint8_t       length;
  } PROGMEM descriptor_list[] = {
  {0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
  {0x0200, 0x0000, config1_descriptor, sizeof(config1_descriptor)},
  {0x2100, GAMEPAD_INTERFACE, config1_descriptor+GAMEPAD_HID_DESC_OFFSET, 9},
  {0x2200, GAMEPAD_INTERFACE, gamepad_hid_report_desc, sizeof(gamepad_hid_report_desc)},
  {0x0300, 0x0000, (const uint8_t *)&string0, 4},
  {0x0301, 0x0409, (const uint8_t *)&string1, sizeof(STR_MANUFACTURER)},
  {0x0302, 0x0409, (const uint8_t *)&string2, sizeof(STR_PRODUCT)}
};

#define NUM_DESC_LIST (sizeof(descriptor_list)/sizeof(struct descriptor_list_struct))

/*
===============
=  Variables  =
===============
*/

// When USB is not configured, usb_configuration=0.
static volatile uint8_t usb_configuration = 0;

uint8_t joystick_x = 128;
uint8_t joystick_y = 128;

uint8_t gamepad_buttons = 0;

static uint8_t gamepad_idle_config = 0;

static uint8_t gamepad_protocol = 1;


// USB initialization.
void usb_init(void) {
  HW_CONFIG();
  USB_FREEZE();                     // Enable USB.
  PLL_CONFIG();                     // Choosing PLL mode.
  while (!(PLLCSR & (1<<PLOCK))) ;  // Waithing for phase lock loop PLL clock.
  USB_CONFIG();                     // Enable OTG and start USB clock.
  UDCON = 0;                        // Enable PULL-UP resistor on D+ for FULL-SPEED.
  usb_configuration = 0;
  USB_INTERRUPT_CONFIG();           // Send empty frames on every 1 milisecond.
  sei();
}

// Return 0 if USB is not configured.
uint8_t usb_configured(void) {
  return usb_configuration;
}

int8_t usb_gamepad_action(uint8_t x, uint8_t y, uint8_t buttons) {
  joystick_x = x;
  joystick_y = y;
  gamepad_buttons = buttons; 

  return usb_gamepad_send();
}

int8_t usb_gamepad_send(void) {
  uint8_t intr_state, timeout;

  if (!usb_configuration) return -1;
  intr_state = SREG;
  cli();
  UENUM = GAMEPAD_ENDPOINT;
  timeout = UDFNUML + 50;
  while (1) {
    // Check ready to send.
    if (UEINTX & (1<<RWAL)) break;
    SREG = intr_state;
    // Turn off USB.
    if (!usb_configuration) return -1;
    // If waiting time is too long.
    if (UDFNUML == timeout) return -1;
    // Ready to new check.
    intr_state = SREG;
    cli();
    UENUM = GAMEPAD_ENDPOINT;
  }
  UEDATX = joystick_x;
  UEDATX = joystick_y;
  UEDATX = gamepad_buttons;
  UEINTX = 0x3A;
  SREG = intr_state;
  return 0;
}

/*
=====================================================
=  Interrupt and request process functions          =
=====================================================
*/

ISR(USB_GEN_vect) {
  uint8_t intbits;

  intbits = UDINT;
  UDINT = 0;
  if (intbits & (1<<EORSTI)) {
    UENUM = 0;
    UECONX = 1;
    UECFG0X = EP_TYPE_CONTROL;
    UECFG1X = EP_SIZE(ENDPOINT0_SIZE) | EP_SINGLE_BUFFER;
    UEIENX = (1<<RXSTPE);
    usb_configuration = 0;
  }
}

// Functions for waiting, ready send and receive packets.
static inline void usb_wait_in_ready(void) {
  while (!(UEINTX & (1<<TXINI))) ;
}

static inline void usb_send_in(void) {
  UEINTX = ~(1<<TXINI);
}

static inline void usb_wait_receive_out(void) {
  while (!(UEINTX & (1<<RXOUTI))) ;
}

static inline void usb_ack_out(void) {
  UEINTX = ~(1<<RXOUTI);
}

// Interrupts to USB Endpoints - processing endpoint 0.
// The rest ot the endpoint are manipulated via functions.
ISR(USB_COM_vect) {
  uint8_t intbits;
  const uint8_t *list;
  const uint8_t *cfg;
  uint8_t i, n, len, en;
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
  uint16_t desc_val;
  const uint8_t *desc_addr;
  uint8_t	desc_length;

  UENUM = 0;
  intbits = UEINTX;
  if (intbits & (1<<RXSTPI)) {
    bmRequestType = UEDATX;
    bRequest = UEDATX;
    wValue = UEDATX;
    wValue |= (UEDATX << 8);
    wIndex = UEDATX;
    wIndex |= (UEDATX << 8);
    wLength = UEDATX;
    wLength |= (UEDATX << 8);
    UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
    if (bRequest == GET_DESCRIPTOR) {
      list = (const uint8_t *)descriptor_list;
      for (i=0; ; i++) {
        if (i >= NUM_DESC_LIST) {
          UECONX = (1<<STALLRQ)|(1<<EPEN);  //stall
          return;
        }
        desc_val = pgm_read_word(list);
        if (desc_val != wValue) {
          list += sizeof(struct descriptor_list_struct);
          continue;
        }
        list += 2;
        desc_val = pgm_read_word(list);
        if (desc_val != wIndex) {
          list += sizeof(struct descriptor_list_struct)-2;
          continue;
        }
        list += 2;
        desc_addr = (const uint8_t *)pgm_read_word(list);
        list += 2;
        desc_length = pgm_read_byte(list);
        break;
      }
      len = (wLength < 256) ? wLength : 255;
      if (len > desc_length) len = desc_length;
      do {
        // Waiting ready host for IN paket.
        do {
          i = UEINTX;
        } while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
        if (i & (1<<RXOUTI)) return;	// Error, the packet is not sent.
        // Send IN packet.
        n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
        for (i = n; i; i--) {
          UEDATX = pgm_read_byte(desc_addr++);
        }
        len -= n;
        usb_send_in();
      } while (len || n == ENDPOINT0_SIZE);
      return;
    }
// Processing from HID requests.
    if (bRequest == SET_ADDRESS) {
      usb_send_in();
      usb_wait_in_ready();
      UDADDR = wValue | (1<<ADDEN);
      return;
    }
    if (bRequest == SET_CONFIGURATION && bmRequestType == 0) {
      usb_configuration = wValue;
      usb_send_in();
      cfg = endpoint_config_table;
      for (i=1; i<5; i++) {
        UENUM = i;
        en = pgm_read_byte(cfg++);
        UECONX = en;
        if (en) {
          UECFG0X = pgm_read_byte(cfg++);
          UECFG1X = pgm_read_byte(cfg++);
        }
      }
      UERST = 0x1E;
      UERST = 0;
      return;
    }
    if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80) {
      usb_wait_in_ready();
      UEDATX = usb_configuration;
      usb_send_in();
      return;
    }

    if (bRequest == GET_STATUS) {
      usb_wait_in_ready();
      i = 0;
      UEDATX = i;
      UEDATX = 0;
      usb_send_in();
      return;
    }
    if (wIndex == GAMEPAD_INTERFACE) {
      if (bmRequestType == 0xA1) {
        if (bRequest == HID_GET_REPORT) {
          usb_wait_in_ready();
          UEDATX = joystick_x;
          UEDATX = joystick_y;
          UEDATX = gamepad_buttons;
          usb_send_in();
          return;
        }
        if (bRequest == HID_GET_IDLE) {
          usb_wait_in_ready();
          UEDATX = gamepad_idle_config;
          usb_send_in();
          return;
        }
        if (bRequest == HID_GET_PROTOCOL) {
          usb_wait_in_ready();
          UEDATX = gamepad_protocol;
          usb_send_in();
          return;
        }
      }
      if (bmRequestType == 0x21) {
        if (bRequest == HID_SET_REPORT) {
          usb_wait_receive_out();
          usb_ack_out();
          usb_send_in();
          return;
        }
        if (bRequest == HID_SET_IDLE) {
          gamepad_idle_config = (wValue >> 8);
          usb_send_in();
          return;
        }
        if (bRequest == HID_SET_PROTOCOL) {
          gamepad_protocol = wValue;
          usb_send_in();
          return;
        }
      }
    }
  }
  UECONX = (1<<STALLRQ) | (1<<EPEN);	// Stopping.
}
