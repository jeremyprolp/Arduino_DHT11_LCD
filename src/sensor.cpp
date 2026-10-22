#include "sensor.h"

#include <Arduino.h>

#include <cmath>

#include <DHT.h>

// ---------------------------------------------------------------------------
// Constants and globals
// ---------------------------------------------------------------------------

namespace
{

constexpr uint8_t kDhtPin = 2;
constexpr uint8_t kDhtType = DHT11;

DHT dht(kDhtPin, kDhtType);

}  // namespace

// ---------------------------------------------------------------------------
// Public functions
// ---------------------------------------------------------------------------

void SensorBegin()
{
  dht.begin();
}

SensorReading ReadSensor()
{
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  bool is_valid = !std::isnan(humidity) && !std::isnan(temperature);

  if (!is_valid)
  {
    return {0.0f, 0.0f, false};
  }
  return {temperature, humidity, true};
}
