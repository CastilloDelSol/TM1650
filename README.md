# TM1650

### 7-segment display driver for JY-MCU module based on TM1650 chip

##### Copyright (c) 2015 Anatoli Arkhipenko

[![arduino-library-badge](https://www.ardu-badge.com/badge/TM1650.svg?)](https://www.ardu-badge.com/TM1650)

### Changelog:

v1.0.0:
- 2015-02-24 - Initial release 

v1.0.1:  
- 2015-04-27 - Added support of program memory (PROGMEM) to store the ASCII to Segment Code table

v1.0.2:
- 2015-08-08 - Added check if panel is connected during init. All calls will be disabled if panel was not connected during init.

v1.1.0:
- 2015-12-20 - Code cleanup. Moved to a single header file. Added Gradual brightness method.

v2.0.0:
- 2024-10-27 - Converted TM1650 class to a template to support both TwoWire (hardware I2C) and SoftwareWire (software I2C) seamlessly.
- Replaced all `byte`, `int`, etc., types with fixed-width types (`uint8_t`, `int16_t`, etc.) for increased portability and clarity.
- Added detailed documentation and examples for both TwoWire and SoftwareWire usage.
