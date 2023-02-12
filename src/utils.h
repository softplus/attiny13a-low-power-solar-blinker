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

/* utils.h */

#ifndef UTILS_H
#define UTILS_H

/* All pin numbers are 0-5, corresponding with PB0 - PB5 */
#define set_pin_output(port) (DDRB |=  (1 << port))
#define set_pin_input(port)  (DDRB &= ~(1 << port))
#define set_pin_high(port)   (PORTB |=  (1 << port))
#define set_pin_low(port)    (PORTB &= ~(1 << port))

/* defines for setting and clearing register bits */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void pin_write_pwm(uint8_t pin, uint8_t value, uint16_t duration_ms);
void delay_us(uint16_t us_count);
void delay_ms(uint16_t ms_count);

void     adc_enable(uint8_t pin);
void     adc_disable();
uint8_t  adc_read();

#endif

