# CLAUDE.md

PlatformIO project for an Arduino UNO R4 WiFi: reads a DHT11 (data on D2) every 2 s and shows temperature/humidity on a 16x2 I2C LCD (address `0x27`). School project (BTS CIEL, U5 "Station Météo"). See `README.md` for wiring and user-facing docs.

## Build

`pio` is **not on PATH**. Use the full path:

```bash
~/.platformio/penv/Scripts/pio.exe run                 # compile (verifies changes; no hardware needed)
~/.platformio/penv/Scripts/pio.exe run -t upload       # flash (needs the board plugged in)
~/.platformio/penv/Scripts/pio.exe device monitor      # serial @ 9600 baud
```

Single env: `uno_r4_wifi` (platform `renesas-ra`, framework `arduino`). Libraries come from `lib_deps` in `platformio.ini`. There are no unit tests (`test/` is the empty template); "verified" means `pio run` succeeds — hardware behaviour can only be checked by the user.

## Layout

- `src/main.cpp` — `setup()`/`loop()`: read interval, consecutive-failure handling, serial logging.
- `src/sensor.cpp` + `include/sensor.h` — `SensorReading`, `SensorBegin()`, `ReadSensor()`; owns the `dht` object and pin constants.
- `src/display.cpp` + `include/display.h` — `DisplayBegin()`, `ShowReadings()`, `ShowSensorError()`; owns `lcd`, the I2C address, and private `PrintRow`/`FormatOneDecimal`.
- Each module keeps its hardware object and constants in an unnamed namespace; headers expose only what `main.cpp` needs and use `#ifndef FILE_H_` guards.
- `misc/` — untracked scratch sketches, **not compiled** (PlatformIO only builds `src/`):
  - `test.cpp` — minimal DHT11-to-serial test.
  - `lcd.cpp` — LCD scrolling-text test (`defiler`).
  - `alt.cpp` — French-commented variant of `main.cpp` that also logs CSV to serial (`LogReadings`). They predate the module split; to try one, it must be the only source in `src/` (move the current files aside).
- `lib/`, `test/` — unused PlatformIO templates.
- Sibling folders `../TP` and `../Rapport` hold the course handout and report (outside this repo).

## Code conventions (match `src/`)

- Google C++ style naming: `kConstantName` for `constexpr` constants, `PascalCase` functions, `snake_case` variables/fields.
- **Allman braces** (opening brace on its own line) and `char *p` pointer style, enforced by `.clang-format` (the VS Code C/C++ extension's clang-format is at `~/.vscode/extensions/ms-vscode.cpptools-*/LLVM/bin/clang-format.exe`). Code should be unchanged by clang-format.
- 2-space indent; section banners `// ----...` separating Constants / Types and globals / Prototypes / Entry points / Helpers.
- Include `<Arduino.h>` explicitly; use C++ headers (`<cmath>`, `<cstdio>`, `std::isnan`, `std::lround`).
- File-local globals, types and helpers live in an unnamed `namespace` (not `static`); helpers are declared in the prototypes block, defined after `setup()`/`loop()`.
- `static_cast`, never C-style casts.
- Non-blocking timing with `millis()` (read once per `loop()`); no `delay()` in `loop()`.
- No `%f` in `snprintf` — float printf isn't supported on this core; use `FormatOneDecimal`.
- Fixed-size `char` buffers instead of `String`; `PrintRow` pads/truncates each LCD row to 16 chars to clear leftovers.
- A sensor error is only shown after `kMaxConsecutiveFailures` failed reads in a row; until then the last good values stay on screen.
- LCD degree sign is `'\xDF'` (HD44780 ROM).
- Comments in `src/` are in English; `misc/alt.cpp` is a heavily commented French version for the school report.

## Gotchas

- If the LCD stays blank, the I2C address may be `0x3F` instead of `0x27` (`kLcdAddress` in `src/display.cpp`).
- DHT11 needs ≥ 2 s between reads; a failed read returns NaN and triggers the "Sensor error / Check wiring" screen.
