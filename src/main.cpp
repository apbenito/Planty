/*
 * LAB Name: Arduino 2.9" E-Paper Display (GxEPD2 Library) - Text Example
 * Author: Khaled Magdy
 * For More Info Visit: www.DeepBlueMbedded.com
*/
#include "sensors.h"
#include "display.h"
#include "wireless.h"
#include <Wire.h>

#define AM2320_ADDR 0x5C  // 7-bit I2C address (0xB8 >> 1)

#define EPD_SS 3
#define EPD_DC 5
#define EPD_RST 17
#define EPD_BUSY 16
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define MAX_DISPLAY_BUFFER_SIZE 800
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) /(EPD::WIDTH / 8))
#define TEXT_LINES 3
#define DATA_PIN 12  // Use any suitable ESP32 GPIO, ensure a 10k pull-up resistor is connected!
#define TIMEOUT_US 1000 // General timeout for safety

static const uint8_t VEML7700_ADDR = 0x10; // 7-bit
static const uint8_t VEML7700_REG_CONF = 0x00;
static const uint8_t VEML7700_REG_ALS = 0x04;

// main.cpp now delegates to sensors/display/wireless modules
void setup() {
  Serial.begin(9600);
  delay(2000);
  bool veml_ok = sensors_begin();
  if (veml_ok) Serial.println("sensors: VEML init ok");
  else Serial.println("sensors: VEML init failed (or not present)");

  display_begin();
  wirelessBegin();
}

void loop() {
  uint16_t als_raw = 0;
  if (read_veml7700_raw(als_raw)) {
    Serial.print("VEML7700 raw ALS: "); Serial.println((float)als_raw);
  } else {
    Serial.println("VEML7700 read failed");
  }

  int moistureValue = read_moisture();
  Serial.print("Soil moisture: "); Serial.println(moistureValue);

  int adcVal = 0;
  float resistance = 0.0f;
  float tempC = read_tempC(adcVal, resistance);
  // Serial.print("ADC raw temp: "); Serial.println(adcVal);
  // Serial.print("Calculated resistance (ohms): "); Serial.println(resistance);
  // Serial.print("Temperature sensor value: "); Serial.println(tempC);
  // Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  PrintMessages(als_raw, moistureValue, tempC);


  readMessagesTelegram();
  String msg = "Current Readings:\n";
  msg += "Light: " + String(als_raw) + "\n";
  msg += "Soil Moisture: " + String(moistureValue) + "\n";
  msg += "Temperature: " + String(tempC) + " C";
  // sendTelegramMessage(msg);
  delay(5000);
}