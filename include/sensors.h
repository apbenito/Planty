#pragma once
#include <Arduino.h>
#include <Wire.h>

// Thermistor / ADC
#define R_FIXED 10000
const int ADC_MAX = 4095;

bool sensorsBegin();
bool readTempMoist(float &humidity, float &temperature);
uint16_t readLightRaw();
int readSoilMoistureRaw(int moistPin);
float readTempC(int tempPin);
