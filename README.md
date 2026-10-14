# DHT11 LCD

Displays temperature and humidity from a DHT11 sensor on a 16x2 I2C LCD, using an Arduino UNO R4 WiFi.

```
Temp: 23.5°C
Hum:  45.0 %
```

## Hardware

| Component         | Connection                                  |
|-------------------|---------------------------------------------|
| Arduino UNO R4 WiFi | Main board                                |
| DHT11 sensor      | Data pin to **D2**, plus 5V and GND         |
| 16x2 LCD (I2C)    | SDA and SCL to the board's I2C pins, plus 5V and GND |

The LCD is expected at I2C address `0x27`. Some modules use `0x3F`. If the screen stays blank, change `kLcdAddress` in `src/main.cpp`.

## Software

Built with [PlatformIO](https://platformio.org/) (VS Code extension or CLI).

Libraries (declared in `platformio.ini`, installed automatically):

- `adafruit/DHT sensor library`
- `marcoschwartz/LiquidCrystal_I2C`

## Build and upload

With the PlatformIO CLI:

```bash
pio run                 # compile
pio run -t upload       # compile and upload to the board
pio device monitor      # open the serial monitor (9600 baud)
```

Or use the PlatformIO buttons in VS Code.

## Behaviour

- The sensor is read every 2 seconds (the DHT11 minimum).
- Temperature and humidity are shown with one decimal place.
- If a read fails, the LCD shows `Sensor error / Check wiring` and a message is printed to the serial monitor.

## Project layout

```
src/main.cpp        Application code
platformio.ini      Board and library configuration
include/, lib/      Project headers and private libraries (unused)
test/               Unit tests (unused)
```

## Notes

- The degree symbol uses character code `0xDF`, which is the standard HD44780 ROM code. Some non-standard LCD modules may show a different character.
- The code follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

## License

No license specified.
