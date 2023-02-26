# Low power ATtiny13A-based solar blinker

[MIT license](LICENSE) / (c) 2023 [John Mueller](https://johnmu.com/)

# Hardware

MCU: ATtiny13A chip [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny13A-Data-Sheet-DS40002307A.pdf)

[PCB, schematic](hardware)

Running at 128kHz, no brown-out detection.

Pins:

* PB0 - LED_D
* PB1 - LED_C
* PB2 - LED_B
* PB3 - LED_A
* PB4 - Sense: ADC via resistor divider on solar cell

Resistor divider:

```
Vinput --+
         |
        [R1]
         |
         +----- Vsense
         |
        [R2]
         |
         +----- GND
```

* Vsense = Vin * R2 / (R1+R2)
* I = Vin / (R1+R2) [Goal: <<1mA]
* Vsense = 1.1V max [ADC Vref set to 1.1V, since VIN is variable]
* Vin ~ 5.5-6V
* -> R2 = R1 * 1.1 / (6 - 1.1) = 0.2245 * R1
* Picking R1 = 1M Ohm -> R2 = 225k Ohm
* I = 6V / 1.2M Ohm = 5nA
* Actual parts measured: R1 = 830k, R2=220k
* Vsense = Vin * 0.21
* ADC = max(255, Vsense * 255/1.1 ) = max(255, Vsense * 232)
* Max Vinput: 255 = Vin * 0.21 * 255 / 1.1 -> Vin = 5.24V (not optimal, but ok)

Mapping measurements to ADC values: 8bit measurement for simplification

| Vi   |  Vs   |  m  | notes |
|------|-------|-----|-------|
| 6.0V | 1.26V | 255 | overflow |
| 5.5V | 1.15V | 255 | overflow |
| 5.0V | 1.05V | 244 | ok |
| 4.5V | 0.95V | 220 | ok |
| 4.0V | 0.84V | 195 | ok |
| 3.0V | 0.63V | 146 | ok |
| 2.5V | 0.53V | 123 | ok |
| 2.0V | 0.42V |  97 | ok |

* Can charge: Vin - 2*diode-forward-voltage > 2.4V
* Roughly max voltage: 5.5v - 2 * 0.7 = 4.1V
* Low voltage for charging: 2.4V = Vin - 1.4V -> Vin = 3.8V
* ADC measurement for charging: 185 (to switch on charging, if we did that)
* ADC measurement for darkness: ca 50 (to switch on lights; experimentally chosen)

# Usage

Compile & upload with platform.io via ICSP

# Anecdotes

TIL

* Works, but is annoying to build & clunky to keep.
* NiMh batteries are great, but totally over-dimensioned

For future revisions:

* Use larger parts (0804 or 1206) for ease of construction. No need for tinyness.
* Use high-visibility / ultra-bright LEDs (High lumen / watt)
* Form-factor for an enclosure, or for something nicer-looking

Things to try:

* measure and document power usage
* consider ADC sleep like http://www.gammon.com.au/forum/?id=12769

# FYI

...