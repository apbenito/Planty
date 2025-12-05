#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  3600        // Sleep for 3600 seconds (1 Hour)

void wirelessBegin();
void readMessagesTelegram();
void fetchPlantInfo();
String urlEncode(String str);
int getPlantIdByName(String plantName);
void sendTelegramMessage(String message);
