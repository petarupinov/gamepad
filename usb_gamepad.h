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

// Definiciq na funkciqta za inicializaciq na USB.
void usb_init(void);

// Definiciq na funkciqta konfiguriran USB port.
uint8_t usb_configured(void);

// Definiciq na funkciqta koqto predava parametri za gamepad-a.
int8_t usb_gamepad_action(uint8_t x, uint8_t y, uint8_t buttons);

// Definiciq na funkciqta koqto izprashta danni.
// EPNUM registara izbira UEDATX registar, koito sadarja 
// 8 dannovi bita, koito se izprashtat kam krainata tochka. 
int8_t usb_gamepad_send(void);

// Definirane na bezznakova promenliva, koqto moje da se 
// izpolzva izvan tozi fail.
extern uint8_t gamepad_buttons;

// Standartni biblioteki ot WinAVR kompilator.
// Biblioteka za vhod i izhod.
#include <avr/io.h>
// Biblioteka za rabota s kodov segment.
#include <avr/pgmspace.h>
// Biblioteka za prekasvaniqta.
#include <avr/interrupt.h>


// Izpolzva se v registar UECFG0X, koito ukazva rejim
// 00b: Control (10b: Bulk, 01b: Isochronous, 11b: Interrupt).
#define EP_TYPE_CONTROL			0x00
// Definirane na stoinost, koqto se izpolzva ot tablicata za 
// krainata tochka. Ot registar UECFG0X se izbira rejim 
// 11b: Interrupt, konfiguraciq za posoka IN na krainata tochka.
#define EP_TYPE_INTERRUPT_IN	0xC1

// Zadelqne na pamet za krainata tochka ot registar UECFG1X.
#define EP_SINGLE_BUFFER		0x02

// Zadelqne na pamet za krainata tochka ot registar UECFG1X
// i zadavane na dvoen bufer s bit 2 - EPBK0 01b: Double bank.
#define EP_DOUBLE_BUFFER		0x06

// Definirane razmera na krainata tochka.
#define EP_SIZE(s)	((s) == 64 ? 0x30 :	\
					((s) == 32 ? 0x20 :	\
					((s) == 16 ? 0x10 :	\
											0x00)))

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)


// Definicii za mikrokontrolera AT90USB1287.
#define HW_CONFIG() (UHWCON = 0x81) // USB ustroistvo i pad regulator.
#define PLL_CONFIG() (PLLCSR = 0x0E)   // Zadavane na chestota za 8MHz.
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE))) // OTG pad.
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK))) // Spirane na takta.

// Krai na reset i zadavane na freim PID (SOF) otkrit vsqka milisekunda.
#define USB_INTERRUPT_CONFIG() (UDIEN = (1<<EORSTE)|(1<<SOFE))

// Standartni tipove zapitvaniq za kontrolni kraini tochki.
#define GET_STATUS				0
#define CLEAR_FEATURE			1
#define SET_FEATURE				3
#define SET_ADDRESS				5
#define GET_DESCRIPTOR			6
#define GET_CONFIGURATION		8
#define SET_CONFIGURATION		9
#define GET_INTERFACE			10
#define SET_INTERFACE			11
// HID (human interface device).
#define HID_GET_REPORT			1	// bRequest
#define HID_GET_IDLE			2
#define HID_GET_PROTOCOL		3
#define HID_SET_REPORT			9
#define HID_SET_IDLE			10
#define HID_SET_PROTOCOL		11
