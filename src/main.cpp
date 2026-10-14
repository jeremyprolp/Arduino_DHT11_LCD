// DHT11 temperature and humidity display.
//
// Board:     Arduino UNO R4 WiFi
// Sensor:    DHT11, data pin connected to D2
// Display:   16x2 LCD with I2C backpack at address 0x27
// Libraries: DHT sensor library (Adafruit), LiquidCrystal_I2C (marcoschwartz)

#include <math.h>
#include <stdio.h>

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

constexpr uint8_t kDhtPin = 2;
constexpr uint8_t kDhtType = DHT11;

constexpr uint8_t kLcdAddress = 0x27;
constexpr uint8_t kLcdColumns = 16;
constexpr uint8_t kLcdRows = 2;

constexpr uint32_t kSerialBaud = 9600;

// The DHT11 is a slow sensor: wait at least 2 seconds between readings.
constexpr uint32_t kReadIntervalMs = 2000;

// Degree sign in the HD44780 character ROM of most LCD modules.
constexpr char kDegreeSymbol = '\xDF';

// e.g. "-12.3" plus null terminator.
constexpr size_t kNumberBufferSize = 8;
// One LCD line plus null terminator.
constexpr size_t kLineBufferSize = kLcdColumns + 1;

// ---------------------------------------------------------------------------
// Types and globals
// ---------------------------------------------------------------------------

struct SensorReading {
  float temperature;
  float humidity;
  bool is_valid;
};

LiquidCrystal_I2C lcd(kLcdAddress, kLcdColumns, kLcdRows);
DHT dht(kDhtPin, kDhtType);

uint32_t last_read_time = 0;

// ---------------------------------------------------------------------------
// Function prototypes
// ---------------------------------------------------------------------------

static void FormatOneDecimal(float value, char* out, size_t size);
static void PrintRow(uint8_t row, const char* text);
static void ShowReadings(float temperature, float humidity);
static void ShowSensorError();
static SensorReading ReadDhtSensor();

// ---------------------------------------------------------------------------
// Arduino entry points (names are fixed by the Arduino framework)
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(kSerialBaud);

  lcd.init();
  lcd.backlight();
  dht.begin();
}

void loop() {
  // Non-blocking timing: only read again once the interval has elapsed.
  if (millis() - last_read_time < kReadIntervalMs) {
    return;
  }
  last_read_time = millis();

  SensorReading reading = ReadDhtSensor();
  if (!reading.is_valid) {
    Serial.println("Failed to read from DHT sensor!");
    ShowSensorError();
    return;
  }

  ShowReadings(reading.temperature, reading.humidity);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Formats a float with one decimal place without using %f,
// because float printf is not supported on this core.
static void FormatOneDecimal(float value, char* out, size_t size) {
  long tenths = (long)(value * 10.0f + (value < 0 ? -0.5f : 0.5f));
  bool negative = tenths < 0;
  if (negative) {
    tenths = -tenths;
  }

  snprintf(out, size, "%s%ld.%ld", negative ? "-" : "", tenths / 10, tenths % 10);
}

// Writes text to one LCD row, padded with spaces to clear leftovers.
static void PrintRow(uint8_t row, const char* text) {
  char line[kLineBufferSize];

  uint8_t i = 0;
  for (; i < kLcdColumns && text[i] != '\0'; i++) {
    line[i] = text[i];
  }
  for (; i < kLcdColumns; i++) {
    line[i] = ' ';
  }
  line[kLcdColumns] = '\0';

  lcd.setCursor(0, row);
  lcd.print(line);
}

static void ShowReadings(float temperature, float humidity) {
  char number[kNumberBufferSize];
  char line[kLineBufferSize];

  FormatOneDecimal(temperature, number, sizeof(number));
  snprintf(line, sizeof(line), "Temp: %s%cC", number, kDegreeSymbol);
  PrintRow(0, line);

  FormatOneDecimal(humidity, number, sizeof(number));
  snprintf(line, sizeof(line), "Hum:  %s %%", number);
  PrintRow(1, line);
}

static void ShowSensorError() {
  PrintRow(0, "Sensor error");
  PrintRow(1, "Check wiring");
}

static SensorReading ReadDhtSensor() {
  SensorReading reading;
  reading.humidity = dht.readHumidity();
  reading.temperature = dht.readTemperature();
  reading.is_valid = !isnan(reading.humidity) && !isnan(reading.temperature);

  if (!reading.is_valid) {
    reading.humidity = 0.0f;
    reading.temperature = 0.0f;
  }

  return reading;
}
