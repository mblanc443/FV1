// Designed by Michael Blanc <<kd2cmo>> New Jersey
Designed as embedded into a Fender Strat along w/ active humbakers pickups powered by Li-Ion rechargables.
Arduino Encoder and 3 buttons to be used in this project. 
Buttons navigate every 10% per click forward only, at 100% jumps to 0% on the next click.

Connections 1.8" ST7735 to Arduino Nano:
OLED    Arduino Nano
LED   - 3.3v
SCK   - D13
SDA   - D11
A0    - D8
RST   - D9
CS    - D10
GND   - GND
VCC   - 5V

Encoder ArdNano
CW    - D2
CCW   - D4

Parameters buttons 1, 2, 3, other button's pins connected to GND
Button 1  - A0
Button 2  - A1
Button 3  - A2

PWM control of FV1 POT inputs:
POT1 - D3
POT2 - D5
POT3 - D6

Outputs:
S0   - A3  // controls selection of effects
S1   - A4
S2   - A5
Rom1 -  0  // selects ext ROMs
Rom2 -  1
ExtRom Enable - 7 // Switches between internal and external ROMs
