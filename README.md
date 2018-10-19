 # Brief
 Arduino reads and prints all data and commands of the PS2 gamepad. Analog Sticks are filtered using [Radial Scaling method](https://www.gamasutra.com/blogs/JoshSutphin/20130416/190541/Doing_Thumbstick_Dead_Zones_Right.php) and printed when there is a change. Makes use of the [PS2X library for Arduino](https://github.com/madsci1016/Arduino-PS2X).
 
 # Board
 Arduino Pro Mini ATmega 328 (3.3V, 8MHz)
 
 # Connections:
| Type | Pro Mini | PS2 controller | Components |
| ---- | -------- | -------------- | ---------- |
| Output | D2 | Data (brown) | R1 |
| Output | D3 | Attention (green) |
| Output | D4 | Command (orange) |
| Output | D5 | Clock (blue) |
| Output | VCC | 3.3V DC | R1
| Output | GND | GND |
