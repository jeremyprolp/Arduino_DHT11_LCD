// DHT11 temperature and humidity display.
//
// Board:     Arduino UNO R4 WiFi
// Sensor:    DHT11, data pin connected to D2           (see sensor.cpp)
// Display:   16x2 LCD with I2C backpack at address 0x27 (see display.cpp)
// Libraries: DHT sensor library (Adafruit), LiquidCrystal_I2C (marcoschwartz)

#include <Arduino.h>

#include "display.h"
#include "sensor.h"

// ---------------------------------------------------------------------------
// Constants and globals
// ---------------------------------------------------------------------------

namespace
{

constexpr uint32_t kSerialBaud = 9600;

// The DHT11 is a slow sensor: wait at least 2 seconds between readings.
constexpr uint32_t kReadIntervalMs = 2000;

// The DHT11 occasionally fails a single read even when wired correctly,
// so only show the error screen after this many failures in a row.
constexpr uint8_t kMaxConsecutiveFailures = 3;

uint32_t last_read_time = 0;
uint8_t consecutive_failures = 0;

}  // namespace

// ---------------------------------------------------------------------------
// Arduino entry points (names are fixed by the Arduino framework)
// ---------------------------------------------------------------------------

void setup()
{
  Serial.begin(kSerialBaud);

  DisplayBegin();
  SensorBegin();
}

void loop()
{
  // Non-blocking timing: only read again once the interval has elapsed.
  uint32_t now = millis();
  if (now - last_read_time < kReadIntervalMs)
  {
    return;
  }
  last_read_time = now;

  SensorReading reading = ReadSensor();
  if (!reading.is_valid)
  {
    Serial.println("Failed to read from DHT sensor!");

    // Keep the last good values on screen until failures persist.
    if (consecutive_failures < kMaxConsecutiveFailures)
    {
      consecutive_failures++;
    }
    if (consecutive_failures >= kMaxConsecutiveFailures)
    {
      ShowSensorError();
    }
    return;
  }

  consecutive_failures = 0;
  ShowReadings(reading);
}
