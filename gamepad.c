/* Keyboard example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 * Copyright (c) 2008 PJRC.COM, LLC
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

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "usb_gamepad.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
/*
USBKEY led description:
-------------------------------------
| svetodiod | USBKEY pin  | colour  |
-------------------------------------
|   D2      |   PORTD_4   | Red     |
|   (down)  |   PORTD_5   | Green   |
-------------------------------------
|   D5      |   PORTD_6   | Green   |
|   (up)    |   PORTD_7   | Red     |
-------------------------------------
*/

// PORTD 4,5,6,7 defined pinouts.
#define LEDs_CONFIG (DDRD  |=  (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7))

// Turn off all leds.
#define LEDs_OFF	(PORTD &= ~(1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7))
// Turn on all leds.
#define LEDs_ON		(PORTD |=  (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7))

#define LED0_OFF	(PORTD &= ~(1<<PD4)) // LED D2 RED OFF
#define LED0_ON		(PORTD |=  (1<<PD4)) // LED D2 RED ON
#define LED1_OFF	(PORTD &= ~(1<<PD5)) // LED D2 GREEN OFF
#define LED1_ON		(PORTD |=  (1<<PD5)) // LED D2 GREEN ON
#define LED2_OFF	(PORTD &= ~(1<<PD6)) // LED D5 GREEN OFF
#define LED2_ON		(PORTD |=  (1<<PD6)) // LED D5 GREEN ON
#define LED3_OFF	(PORTD &= ~(1<<PD7)) // LED D5 RED OFF
#define LED3_ON		(PORTD |=  (1<<PD7)) // LED D5 RED ON

/*
USBKEY joystick buttons description:
---------------------------------------------------------
|  button  |  USBKEY pins  |  external PULLUP resistor  |
---------------------------------------------------------
|  SELECT  |    PORTB_5    |            no              |
|  LEFT    |    PORTB_6    |            no              |
|  UP      |    PORTB_7    |            no              |
|  RIGHT   |    PORTE_4    |            no              |
|  DOWN    |    PORTE_5    |            no              |
---------------------------------------------------------
*/

#define JOYSTICK_SELECT (1 << 5)   // PB5=1
#define JOYSTICK_LEFT   (1 << 6)   // PB6=1
#define JOYSTICK_UP     (1 << 7)   // PB7=1
#define JOYSTICK_RIGHT  (1 << 4)   // PE4=1
#define JOYSTICK_DOWN   (1 << 5)   // PE5=1

// PINB=(1<<PB6)+(1<<PB7).
#define JOYSTICK_PINSB (JOYSTICK_LEFT  | JOYSTICK_UP  )
// PINE=(1<<PE4)+(1<<PE5).
#define JOYSTICK_PINSE (JOYSTICK_RIGHT | JOYSTICK_DOWN)

/*
USBKEY buttons:
---------------------------------------------------------
|   buton   |  USBKEY pins  | external pull up resistor |
---------------------------------------------------------
|(1)SELECT  |    PORTB_5    |           no              |
|(3)HWB     |    PORTE_2    |           yes             |
---------------------------------------------------------
*/

// Buton 1, PINB=(1<<PB5).
#define BUTTON1 JOYSTICK_SELECT
// Buton 2, PINE=(1<<PE2).
#define BUTTON2 (1 << 2)

// JOYSTICK_SELECT + HWB Buton.
#define BUTTON_PINS	(BUTTON1 | BUTTON2 )

// Main function.
int main(void) {
// Gamepad button directions.
  uint8_t btn, x, y;

// Prescaller setup to 8MHz clock, CLKPR = 0b00000000.
  CPU_PRESCALE(0);

// Configure PORTD as output.
  LEDs_CONFIG;
// PORTD leds test.
  LED2_ON;
  _delay_ms(250);
  LED2_OFF;
  LED3_ON;
  _delay_ms(250);
  LED3_OFF;
  LED0_ON;
  _delay_ms(250);
  LED0_OFF;
  LED1_ON;

// Enable internal pull up resistors on PORTB.
  PORTB = (JOYSTICK_PINSB | BUTTON_PINS);
// Enable internal pull up resistors on PORTE.
  PORTE = (JOYSTICK_PINSE);

// USB initialization function.
  usb_init();
// Wait for usb configuration, waiting for ready.
  while (!usb_configured());

// Waiting 1 second to OS loading drivers.
  _delay_ms(1000);

// Forever loop.
  while (1) {
// Buttons initialization value = 0.
    btn = 0;
// Initialization value for X,Y = 128,
// Zero point is in the middle 0 - 128 - 255.
    x = y = 128;

// Controls - X: lerf and right.
    if ((PINB & JOYSTICK_LEFT) == 0) {
      x = 0;
    } else if ((PINE & JOYSTICK_RIGHT) == 0) {
      x = 255;
    }
// Controls - Y: up and down.
    if ((PINB & JOYSTICK_UP) == 0) {
      y = 0;
    } else if ((PINE & JOYSTICK_DOWN) == 0) {
      y = 255;
    }

// Buttons 1 and 2, btn returns buttons number or both.
// Check thogether pressed buttons 1 and 2.
    if (((PINB & BUTTON1) == 0) && ((PINE & BUTTON2) == 0)) {
      btn = 3;                      // buttons 1 and 2 together pressed
    } else {
// Check pressed only button 1.
      if ((PINB & BUTTON1) == 0) {  // pinb5 == test BUTTON1
        btn = 1;                    // JOYSTICK SELECT - buton1
      }
// Check pressed only button 2.
      if ((PINE & BUTTON2) == 0) {  // pine2 == test BUTTON2
        btn = 2;                    // HWB - buton2
      }
    }

// Call func to send parameters.
    usb_gamepad_action(x, y, btn);

// If pressed button LED3 is turned on.
    if (x != 128 || y != 128 || btn != 0) {
      LED3_ON;
    } else {
// If no pressed button LED3 is turned off.
      LED3_OFF;
    }
  }
}

