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
/*
F_CPU e konstanta, stoinostta na koqta opredelq koeficienta na delenie 
na osnovnata rabotna chestota na mikrokontrolera. Ako toi e nastroen 
da raboti s 8MHz i F_CPU e 1000000UL [Hz], to rabotnata chestota shte 
bade 8MHz. Ako toi e nastroen da raboti s 8MHz i F_CPU e 8000000UL [Hz],
to rabotnata chestota shte bade 1MHz. Po podrazbirane F_CPU e definirana 
sas stoinost 1000000UL, t.e. 1MHz vav faila "util/delay.h", v koito se 
pravi proverka dali veche e bila definirana konstantata F_CPU s nqkakva 
stoinost ili ne e imalo definiciq na F_CPU. Failat "delay.h" se namira 
v poddirektoriqta /util v direktoriqta na instaliraniq WinAVR kompilator.
*/
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "usb_gamepad.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
/*
USBKEY svetodiodi:
-------------------------------------
| svetodiod	| USBKEY izvod	| cvqt	|
-------------------------------------
|	D2		|	PORTD_4		| Red	|
|	(dolu)	|	PORTD_5		| Green	|
-------------------------------------
|	D5		|	PORTD_6 	| Green	|
|	(gore)	|	PORTD_7		| Red	|
-------------------------------------
*/

// PORTD 4,5,6,7 sa definirani izhodi.
#define LEDs_CONFIG (DDRD  |=  (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7))

// Izgasva vsichki svetodiodi.
#define LEDs_OFF	(PORTD &= ~(1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7))
// Svetva vsichki svetodiodi.
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
USBKEY joystick:
---------------------------------------------------------
|	buton	| USBKEY izvod	| vanshen PULLUP rezistor	|
---------------------------------------------------------
|	SELECT	|	PORTB_5		|			ne				|
|	LEFT	|	PORTB_6		|			ne				|
|	UP		|	PORTB_7		|			ne				|
|	RIGHT	|	PORTE_4		|			ne				|
|	DOWN	|	PORTE_5		|			ne				|
---------------------------------------------------------
*/

#define JOYSTICK_SELECT	(1 << 5)	// PB5=1
#define JOYSTICK_LEFT	(1 << 6)	// PB6=1
#define JOYSTICK_UP		(1 << 7)	// PB7=1
#define JOYSTICK_RIGHT	(1 << 4)	// PE4=1
#define JOYSTICK_DOWN	(1 << 5)	// PE5=1

// Sadarja PINB=(1<<PB6)+(1<<PB7).
#define JOYSTICK_PINSB (JOYSTICK_LEFT  | JOYSTICK_UP  )
// Sadarja PINE=(1<<PE4)+(1<<PE5).
#define JOYSTICK_PINSE (JOYSTICK_RIGHT | JOYSTICK_DOWN)

/*
USBKEY buttons:
---------------------------------------------------------
|	buton	| USBKEY izvod	| vanshen PULLUP rezistor	|
---------------------------------------------------------
|(1)SELECT	|	PORTB_5		|			ne				|
|(3)HWB		|	PORTE_2		|			da				|
---------------------------------------------------------
*/

// Buton 1, sadarja PINB=(1<<PB5).
#define BUTTON1 JOYSTICK_SELECT
// Buton 2, sadarja PINE=(1<<PE2).
#define BUTTON2 (1 << 2)

// JOYSTICK_SELECT + HWB Buton.
#define BUTTON_PINS	(BUTTON1 | BUTTON2 )

// GLAVNA FUNKCIA.
int main(void) {
// Definicia za posoki i butoni na Gamepad-a.
  uint8_t btn, x, y;

// Nastroika za preddelitelq za 8MHz takt, CLKPR = 0b00000000.
  CPU_PRESCALE(0);

// Konfigurirane na PORTD kato izhod.
  LEDs_CONFIG;
// Test na svetodiodi ot PORTD.
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

// Razreshavane na vatreshnite izteglqshti rezistori na PORTB.
  PORTB = (JOYSTICK_PINSB | BUTTON_PINS);
// Razreshavane na vatreshnite izteglqshti rezistori na PORTE.
  PORTE = (JOYSTICK_PINSE);

// Inicializirashta funkciq za USB-to.
  usb_init();
// Izchakvane za konfiguraciq, izchakva hosta da ogovori.
  while (!usb_configured());

// Izchakvane na 1 sekunda (nujno vreme na OS za zarejdane na draivari),
// predi zapochvaneto na bezkrainiq cikal s proverki.
  _delay_ms(1000);

// Vechen cikal za proverki.
  while (1) {
// Inicializirashta stoinost za butoni = 0.
    btn = 0;
// Inicializirashta stoinost za X,Y osi = 128,
// srednata tochka ili tochkata O na koordinatnata os.
    x = y = 128;

// Kontroli - os X: nalqvo i nadqsno.
    if ((PINB & JOYSTICK_LEFT) == 0) {
      x = 0;
    } else if ((PINE & JOYSTICK_RIGHT) == 0) {
      x = 255;
    }
// Kontroli - os Y: nagore i nadolu.
    if ((PINB & JOYSTICK_UP) == 0) {
      y = 0;
    } else if ((PINE & JOYSTICK_DOWN) == 0) {
      y = 255;
    }

// BUTONI 1 i 2, btn vrashta nomera na butona ili dvata zaedno.
// Proverka za natisnati dvata butona 1 i 2 navednaj.
	if (((PINB & BUTTON1) == 0) && ((PINE & BUTTON2) == 0)) {
		btn = 3;						// buton1+buton2
	}
	else {
// Proverka za natisnat buton 1.
		if ((PINB & BUTTON1) == 0) {	// pinb5 == test BUTTON1
		btn = 1;					// JOYSTICK SELECT - buton1
		}
// Proverka za natisnat buton 2.
		if ((PINE & BUTTON2) == 0) {	// pine2 == test BUTTON2
		btn = 2;						// HWB - buton2
		}
	}

// Izvikva se funkciqta za predavane na parametri.
    usb_gamepad_action(x, y, btn);

// Ako ima natisnat buton se svetva LED3.
	if (x != 128 || y != 128 || btn != 0) {
      LED3_ON;
    }
// Ako nqma natisnat buton ne svetva LED3.
	else {
      LED3_OFF;
    }
  }
}
 
