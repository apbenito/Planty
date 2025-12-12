#include "sensors.h"
#include "main.h"
#include <Wire.h>

// Absolute readings:
// Soil moisture: 1250 - 4096
// Light: 0 - 65535, 8-10k is ambient light

static const uint8_t AM2320_ADDR = 0x5C;  // 7-bit I2C address
static const uint8_t VEML7700_ADDR = 0x10; // 7-bit
static const uint8_t VEML7700_REG_CONF = 0x00;
static const uint8_t VEML7700_REG_ALS = 0x04;

// Steinhart constants (keep as in original)
const float A = 8.202819e-04;
const float B = 2.636000e-04;
const float C = 1.360785e-07;

// CRC16 (Modbus) for AM2320 reply
static uint16_t crc16_modbus(const uint8_t *buf, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc ^= buf[i];
        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 1) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }
    return crc;
}

bool readTempMoist(float &humidity, float &temperature) {
    uint8_t resp[8];

    // Wake sensor (dummy write)
    Wire.beginTransmission(AM2320_ADDR);
    uint8_t wake_status = Wire.endTransmission();
    Serial.println("Wake status: " + String(wake_status));
    delay(2); // >800µs recommended

    // Request 4 registers starting at 0x00 (humidity + temperature)
    Wire.beginTransmission(AM2320_ADDR);
    Wire.write(0x03); // function: read holding registers
    Wire.write(0x00); // start addr
    Wire.write(0x04); // number of regs
    uint8_t tx_status = Wire.endTransmission();
    Serial.println("Transmission status: " + String(tx_status));
    if (tx_status != 0) {
      Serial.println("I2C Transmission Error, error code: " + String(tx_status));
      return false;
    }

    // Read 8 bytes: func, bytecount, humH, humL, tmpH, tmpL, CRC_L, CRC_H
    uint8_t bytes_read = Wire.requestFrom(AM2320_ADDR, (uint8_t)8);
    Serial.println("Bytes requested: 8, Bytes received: " + String(bytes_read));
    if (bytes_read != 8) {
      Serial.println("I2C Read Error"); 
      return false;
    }
    for (uint8_t i = 0; i < 8; ++i) resp[i] = Wire.read();

    // CRC check (first 6 bytes)
    uint16_t crc = crc16_modbus(resp, 6);
    uint16_t crc_rx = (uint16_t)resp[7] << 8 | resp[6]; // received CRC is low byte first
    if (crc != crc_rx){
      Serial.println("CRC Mismatch");
      return false;
    } 

    // Parse humidity
    uint16_t hum_raw = (uint16_t)resp[2] << 8 | resp[3];
    humidity = hum_raw / 10.0f;

    // Parse temperature (signed 16-bit, MSB sign bit)
    int16_t tmp_raw = (int16_t)((resp[4] << 8) | resp[5]);
    if (tmp_raw & 0x8000) tmp_raw = -(tmp_raw & 0x7FFF);
    temperature = tmp_raw / 10.0f;

    return true;
}

// VEML7700 helpers
bool read16Reg(uint8_t addr, uint8_t reg, uint16_t &out) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false; // restart
  uint8_t n = Wire.requestFrom((uint8_t)addr, (uint8_t)2);
  if (n != 2) return false;
  uint8_t l = Wire.read();
  uint8_t h = Wire.read();
  out = (uint16_t)h << 8 | l;
  return true;
}

bool write16reg(uint8_t addr, uint8_t reg, uint16_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value & 0xFF);        // LSB
  Wire.write((value >> 8) & 0xFF); // MSB
  return (Wire.endTransmission() == 0);
}

bool initLightSensor() {
    Wire.beginTransmission(VEML7700_ADDR);
    uint16_t conf = 0x0000;
    Wire.write(VEML7700_REG_CONF);
    Wire.write(conf & 0xFF);
    Wire.write((conf >> 8) & 0xFF);
    return (Wire.endTransmission() == 0);
}

uint16_t readLightRaw() {
    Wire.beginTransmission(VEML7700_ADDR);
    Wire.write(VEML7700_REG_ALS);
    if (Wire.endTransmission(false) != 0) return 0; // send restart
    uint8_t n = Wire.requestFrom(VEML7700_ADDR, (uint8_t)2);
    if (n != 2) return 0;
    uint8_t l = Wire.read();
    uint8_t h = Wire.read();
    return (uint16_t)h << 8 | l;
}

float calculateTempSteinhart(float r) {
  if (r <= 0) return -999.0; 
  float logR = log(r);
  float inverseT = A + B * logR + C * pow(logR, 3);
  return (1.0 / inverseT) - 273.15;
}

bool sensorsBegin() {
  // Initialize I2C on the ESP32 default pins used earlier
  Wire.begin(21, 22);
  // set pin modes
  pinMode(MOIST_PIN_CALATHEA, INPUT);
  pinMode(MOIST_PIN_DIEFFENBACHIA, INPUT);
  pinMode(TEMP_PIN, INPUT);
  analogSetPinAttenuation(TEMP_PIN, ADC_11db);
  // Try to initialize VEML
  return initLightSensor();
}

int readSoilMoistureRaw(int moistPin) {
  return analogRead(moistPin);
}

float readTempC(int tempPin) {
  int adcOut = analogRead(tempPin);
  int resistanceOut;
  if (adcOut <= 0 || adcOut >= ADC_MAX) {
    resistanceOut = -1.0;
    return -999.0;
  }
  resistanceOut = R_FIXED * ((float)adcOut / (ADC_MAX - adcOut));
  return calculateTempSteinhart(resistanceOut);
}
