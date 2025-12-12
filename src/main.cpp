/*
 * Planty Firmware V1.0
 * Author: Jorian Rennen, Pablo Arellano
*/
#include "sensors.h"
#include "display.h"
#include "wireless.h"
#include "main.h"
#include <Wire.h>
#include <ArduinoJson.h>

JsonDocument logData;
char logJsonString[256];

// main.cpp now delegates to sensors/display/wireless modules
void setup() {
  Serial.begin(9600);
  delay(2000);
  bool sensorsOk = sensorsBegin();
  if (sensorsOk) Serial.println("sensors: VEML init ok");
  else Serial.println("sensors: VEML init failed (or not present)");

  einkBegin();
  wirelessBegin();
}

void loop() {
  uint16_t lightValue = readLightRaw();
  int moistureValueCalathea = readSoilMoistureRaw(MOIST_PIN_CALATHEA);
  int moistureValueDieffenbachia = readSoilMoistureRaw(MOIST_PIN_DIEFFENBACHIA);
  float tempC = readTempC(TEMP_PIN);

  Serial.print("\n\nLight raw: "); Serial.println(lightValue);
  Serial.print("Soil moisture Calathea: "); Serial.println(moistureValueCalathea);
  Serial.print("Soil moisture Dieffenbachia: "); Serial.println(moistureValueDieffenbachia);
  Serial.print("Temperature C: "); Serial.println(tempC);

  Serial.println("Updating E-Ink display...");
  printMessagesEink(lightValue, moistureValueCalathea, moistureValueDieffenbachia, tempC);

  logData["lightAmbient"] = lightValue;
  logData["moistureCalathea"] = moistureValueCalathea;
  logData["moistureDieffenbachia"] = moistureValueDieffenbachia;
  logData["temperatureAmbient"] = tempC;

  serializeJson(logData, logJsonString);
  sendTelegramMessage(logJsonString);

  Serial.println("Delay 15 mins...");
  delay(900000);
}