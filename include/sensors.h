#pragma once
#include <Arduino.h>
#include <Wire.h>

// Pins
#define MOIST_PIN 34
#define TEMP_PIN 32 // ADC1 pin (GPIO32)

// Thermistor / ADC
#define R_FIXED 10000
const int ADC_MAX = 4095;

bool sensors_begin();
bool read_am2320_i2c(float &humidity, float &temperature);
bool read_veml7700_raw(uint16_t &als_raw);
int read_moisture();
float read_tempC(int &adcOut, float &resistanceOut);
