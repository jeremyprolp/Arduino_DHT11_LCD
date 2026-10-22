#include "display.h"

#include <Arduino.h>

#include <cmath>
#include <cstdio>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// ---------------------------------------------------------------------------
// Constants and globals
// ---------------------------------------------------------------------------

namespace
{

constexpr uint8_t kLcdAddress = 0x27;
constexpr uint8_t kLcdColumns = 16;
constexpr uint8_t kLcdRows = 2;

// Degree sign in the HD44780 character ROM of most LCD modules.
constexpr char kDegreeSymbol = '\xDF';

// e.g. "-12.3" plus null terminator.
constexpr size_t kNumberBufferSize = 8;
// One LCD line plus null terminator.
constexpr size_t kLineBufferSize = kLcdColumns + 1;

LiquidCrystal_I2C lcd(kLcdAddress, kLcdColumns, kLcdRows);

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

// Formats a float with one decimal place without using %f,
// because float printf is not supported on this core.
void FormatOneDecimal(float value, char *out, size_t size)
{
  long tenths = std::lround(value * 10.0f);
  bool negative = tenths < 0;
  if (negative)
  {
    tenths = -tenths;
  }

  snprintf(out, size, "%s%ld.%ld", negative ? "-" : "", tenths / 10, tenths % 10);
}

// Writes text to one LCD row, padded with spaces (or truncated) to the full
// width so leftovers from the previous text are cleared.
void PrintRow(uint8_t row, const char *text)
{
  char line[kLineBufferSize];
  snprintf(line, sizeof(line), "%-*s", kLcdColumns, text);

  lcd.setCursor(0, row);
  lcd.print(line);
}

}  // namespace

// ---------------------------------------------------------------------------
// Public functions
// ---------------------------------------------------------------------------

void DisplayBegin()
{
  lcd.init();
  lcd.backlight();
  PrintRow(0, "Starting...");
}

void ShowReadings(const SensorReading &reading)
{
  char number[kNumberBufferSize];
  char line[kLineBufferSize];

  FormatOneDecimal(reading.temperature, number, sizeof(number));
  snprintf(line, sizeof(line), "Temp: %s%cC", number, kDegreeSymbol);
  PrintRow(0, line);

  FormatOneDecimal(reading.humidity, number, sizeof(number));
  snprintf(line, sizeof(line), "Hum:  %s %%", number);
  PrintRow(1, line);
}

void ShowSensorError()
{
  PrintRow(0, "Sensor error");
  PrintRow(1, "Check wiring");
}
