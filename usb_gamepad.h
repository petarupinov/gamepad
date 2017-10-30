/*
PLL - Phase Lock Loop
PLL Prescaller Configuration Register
------------------------------------------------
| - | - | - |PLLP2 |PLLP1 |PLLP0 |PLLE | PLOCK | == PLLCSR
------------------------------------------------

(FOSC==8000)   ===> ( (0<<PLLP2) | (1<<PLLP1) | (1<<PLLP0) ) == 0x03 PLLCFG

0x0E = xxx01110 // 8MHz
*/

#include <stdint.h>

// Define function for USB initialization.
void usb_init(void);

// Define function for configured USB port.
uint8_t usb_configured(void);

// Define func which send params for gamepad.
int8_t usb_gamepad_action(uint8_t x, uint8_t y, uint8_t buttons);

// Define function which send data.
// EPNUM register use UEDATX registar
// 8 bit data, which send to endpoint.
int8_t usb_gamepad_send(void);

// Define unsigned variable for external using
// outside from this file.
extern uint8_t gamepad_buttons;

// Standard libraries for WinAVR compiler.
// I/O AVR library.
#include <avr/io.h>
// Biblioteka za rabota s kodov segment.
#include <avr/pgmspace.h>
// Biblioteka za prekasvaniqta.
#include <avr/interrupt.h>


// Using in register UECFG0X:
// 00b: Control (10b: Bulk, 01b: Isochronous, 11b: Interrupt).
#define EP_TYPE_CONTROL                 0x00
// Define value that using in table for endpoint.
// Using mode from register UECFG0X:
// 11b: Interrupt, configuration for direction fot IN endpoint.
#define EP_TYPE_INTERRUPT_IN            0xC1

// Set memory for endpoint in registar UECFG1X.
#define EP_SINGLE_BUFFER                0x02

// Set memory for endpoint in registar UECFG1X
// using double buffer with bit 2 - EPBK0 01b: Double bank.
#define EP_DOUBLE_BUFFER                0x06

// Define size for endpoint.
#define EP_SIZE(s)  ((s) == 64 ? 0x30 : \
                                        ((s) == 32 ? 0x20 : \
                                        ((s) == 16 ? 0x10 : \
                                                            0x00)))

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)


// Definition for AT90USB1287.
#define HW_CONFIG() (UHWCON = 0x81)                      // USB device and pad regulator.
#define PLL_CONFIG() (PLLCSR = 0x0E)                     // Set clock 8MHz.
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE))) // OTG pad.
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))  // Stop clock.

// End of reset and set frame PID (SOF).
#define USB_INTERRUPT_CONFIG() (UDIEN = (1<<EORSTE)|(1<<SOFE))

// Standard request for endpoints.
#define GET_STATUS                      0
#define CLEAR_FEATURE                   1
#define SET_FEATURE                     3
#define SET_ADDRESS                     5
#define GET_DESCRIPTOR                  6
#define GET_CONFIGURATION               8
#define SET_CONFIGURATION               9
#define GET_INTERFACE                   10
#define SET_INTERFACE                   11
// HID (human interface device).
#define HID_GET_REPORT                  1       // bRequest
#define HID_GET_IDLE                    2
#define HID_GET_PROTOCOL                3
#define HID_SET_REPORT                  9
#define HID_SET_IDLE                    10
#define HID_SET_PROTOCOL                11
