// DHT11 temperature and humidity sensor.

#ifndef SENSOR_H_
#define SENSOR_H_

struct SensorReading
{
  float temperature;
  float humidity;
  bool is_valid;
};

// Starts the sensor. Call once from setup().
void SensorBegin();

// Reads the sensor. Wait at least 2 seconds between calls (DHT11 limit).
SensorReading ReadSensor();

#endif  // SENSOR_H_
