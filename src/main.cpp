/*
	MIT License

	Copyright (c) 2022-2023 John Mueller

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

/* main.cpp */

/* Main function loops. */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "main.h"
#include "utils.h"

#define ADC_PIN 4
#define ADC_THRESHOLD 50 // -> ca 1.3v solar cell (pretty dark)

// pin = (led numbers) color
// 0 = (4,8,12) yellow, very darkest
// 1 = (3,7,11) white, very dark
// 2 = (2,6,10) red, very dark
// 3 = (1,5,9)  green

/* Set watchdog timer to 8 seconds */
void set_watchdog() {
	// Docs: https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny13A-Data-Sheet-DS40002307A.pdf#page=49
	// prescale timer to 8s
	BIT_SET(WDTCR, WDP3); BIT_CLEAR(WDTCR, WDP2); BIT_CLEAR(WDTCR, WDP1); BIT_SET(WDTCR, WDP0);
	// Enable watchdog timer interrupts
	BIT_SET(WDTCR, WDTIE);
	sei(); // Enable global interrupts
	// Use the Power Down sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	// ready for sleep_mode();
}


uint8_t pin = 0;
/* blinks the next light as appropriate */
void blinken_light() {
	pin++; if(pin>3) pin=0;
	set_pin_output(pin);
	pin_write_pwm(pin, 50, 250);
}

/* Enables and reads the ADC from the solar panel */
uint8_t read_value() {
	uint8_t adc_value;
	adc_enable(ADC_PIN);
	//delay_us(200); 			// not needed (unclear if needed for accuracy)
	adc_value = adc_read(); 	// potentially discard first and retry? 
	//delay_us(200); 			// not needed (unclear if needed for accuracy)
	//adc_value = adc_read();  	// not needed (unclear if needed for accuracy)
	adc_disable();
	return adc_value;
}

/* Watchdog timer function is empty */
ISR(WDT_vect) { } // empty -> continues in main loop

/* main loop: blink, then go to sleep if appropriate */
int main(void) {
	// prepare power-down sleep mode, watchdog timer set
	set_watchdog();
	for (;;) { // our main loop
		blinken_light();
		if (pin==0) {
			// every time light cycles, check brightness
			// stay asleep if daylight
			while (read_value() > ADC_THRESHOLD) sleep_mode();   
		}
	}
}
