// 16x2 I2C LCD showing the sensor readings.

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "sensor.h"

// Starts the LCD and shows a startup message. Call once from setup().
void DisplayBegin();

void ShowReadings(const SensorReading &reading);
void ShowSensorError();

#endif  // DISPLAY_H_
