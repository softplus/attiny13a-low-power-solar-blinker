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

/* utils.cpp */

#include <avr/io.h>
#include <util/delay.h>
#include "utils.h"

/* Software-PWM on the pin for a given duration, returns when complete */
void pin_write_pwm(uint8_t pin, uint8_t value, uint16_t duration_ms) {
    // value = 0-255, duration = ms
    #define CYCLE_LENGTH 10 // in ms (10ms = 100Hz)
    #define PULSE_US 39 // (CYCLE_LENGTH * 1000/256) = 10ms*1000/256=ca 39us to fit value in 10ms(100Hz)
    // min pulse length at 128khz

    uint8_t on_time = PULSE_US * value;
    uint16_t cycle_counts = duration_ms / CYCLE_LENGTH;
    for (uint16_t i=0; i<cycle_counts; i++) {
        set_pin_high(pin);
        delay_us(on_time);
        set_pin_low(pin);
        delay_us(CYCLE_LENGTH * 1000 - on_time);
    }
}

/* Delay for a given number of microseconds (us) */
void delay_us(uint16_t us_count) {
#if F_CPU>1000000L
#define DELAY_US_ACCURACY 16
#else
#define DELAY_US_ACCURACY 64
#endif
    for (uint16_t i=0; i<us_count/DELAY_US_ACCURACY; i++) 
        _delay_us(DELAY_US_ACCURACY); 
    // 16us accuracy for 1Mhz+, 64us for <1MHz; arbitrarily chosen
}

/* Delay for a given number of milliseconds (ms) */
void delay_ms(uint16_t ms_count) {
    for (uint16_t i=0; i<ms_count; i++) _delay_ms(1);
}

// ADC: https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny13A-Data-Sheet-DS40002307A.pdf#page=89 
/* enable ADC for the given pin */
void adc_enable(uint8_t pin) {
    // only possible for pins 2-5 (PB2-PB5)
    if (pin<2) return;

    BIT_SET(ADMUX, REFS0);  // 0: Vref = Vcc; 1: Vref = 1.1V

    // Select the ADC input
    BIT_CLEAR(ADMUX, MUX0); BIT_CLEAR(ADMUX, MUX1);     // reset to 00
    if ((3==pin) || (4==pin)) BIT_SET(ADMUX, MUX1); // set as needed
    if ((2==pin) || (5==pin)) BIT_SET(ADMUX, MUX0);

    BIT_SET(ADMUX, ADLAR); // left-adjust result -> 8 bit results

    /* Set the prescaler to something in range of 50kHz - 200kHz
    ** 
    ** F_CPU   | Prescalers
    ** 128 kHz   2 (64 kHz) 
    ** 600 kHz   4 (150 kHz), 8 (75 kHz)  [4.8MHz/8]
    ** 1.2 MHz   8 (150 kHz), 16 (75 kHz) [9.6MHz/8]
    ** 4.8 MHz   32 (150 kHz), 64 (75 kHz)
    ** 9.6 MHz   64 (150 kHz), 128 (75 kHz)
    */
    #if F_CPU<360000L
    BIT_CLEAR(ADCSRA, ADPS2); BIT_CLEAR(ADCSRA, ADPS1); BIT_SET(ADCSRA, ADPS0); // div 2 until 360 kHz
    #elif F_CPU<720000L
    BIT_CLEAR(ADCSRA, ADPS2); BIT_SET(ADCSRA, ADPS1); BIT_CLEAR(ADCSRA, ADPS0); // div 4 until 720 kHz
    #elif F_CPU<1440000L
    BIT_CLEAR(ADCSRA, ADPS2); BIT_SET(ADCSRA, ADPS1); BIT_SET(ADCSRA, ADPS0); // div 8 until 1.440 MHz
    #elif F_CPU<2880000L
    BIT_SET(ADCSRA, ADPS2); BIT_CLEAR(ADCSRA, ADPS1); BIT_CLEAR(ADCSRA, ADPS0); // div 16 until 2.880 MHz
    #elif F_CPU<5760000L
    BIT_SET(ADCSRA, ADPS2); BIT_CLEAR(ADCSRA, ADPS1); BIT_SET(ADCSRA, ADPS0); // div 32 until 5.760 MHz
    #else
    BIT_SET(ADCSRA, ADPS2); BIT_SET(ADCSRA, ADPS1); BIT_CLEAR(ADCSRA, ADPS0); // div 64 for rest
    #endif

    BIT_SET(ADCSRA, ADEN);  // enable ADC
}

/* disable ADC for all pins; saves power */
void adc_disable() {
    // disable ADC
    BIT_CLEAR(ADCSRA, ADEN);
}

/* read ADC value (8 bit) */
uint8_t adc_read() {
    // Start the conversion
    BIT_SET(ADCSRA, ADSC);

    // Wait for it to finish
    while (ADCSRA & (1 << ADSC)) { };
    return ADCH;
}
