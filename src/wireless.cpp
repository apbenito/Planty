#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "wireless.h"


// --- CONFIG ---
// WiFi Config
const char* ssid = "GordasUnidas 2.4 GHz";         // replace with your network SSID (name)
const char* password = "tontoelquelolea"; // replace with your network password

// Telegram Config
const char* BOTtoken = "8432113685:AAHDf1IHAU55f8bffvOdNkU9YmRwb152Lo0"; 
const char* CHAT_ID = "-5023780399"; // Security: Only obey YOU

// Plant API key
const char* PLANT_API_KEY = "sk-lI5v6931f70190d5713826";

// Objects
WiFiClientSecure telegramClient;
UniversalTelegramBot bot(BOTtoken, telegramClient);

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  3600        // Sleep for 3600 seconds (1 Hour)

RTC_DATA_ATTR String storedPlantName = "Unknown"; // Saved in "RTC Memory" (survives sleep)

void wirelessBegin();
void readMessagesTelegram();
// void fetchPlantInfo(){};
// String urlEncode(String str){};
// int getPlantIdByName(String plantName){};
void sendTelegramMessage(String message);

void wirelessBegin() {

    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to WiFi SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    telegramClient.setInsecure(); // Disable certificate validation
}

void readMessagesTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  
  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;
    
    if (text.startsWith("/plant ")) {
    //   storedPlantName = text.substring(7);
      bot.sendMessage(CHAT_ID, "Plant configuration updated to: " + text, "");
    }
    else {
      bot.sendMessage(CHAT_ID, "Unknown command. Use /plant <PlantName> to set the plant.", "");
      Serial.println("Unknown command received: " + text);
    }
  }
}

// void fetchPlantInfo() {
    
// }

// // Helper to replace spaces with '+' for URLs (e.g., "Silver Fir" -> "Silver+Fir")
// String urlEncode(String str) {
//   String encodedString = "";
//   char c;
//   char code0;
//   char code1;
//   for (int i = 0; i < str.length(); i++) {
//     c = str.charAt(i);
//     if (c == ' ') {
//       encodedString += '+';
//     } else {
//       encodedString += c; 
//     }
//   }
//   return encodedString;
// }

// int getPlantIdByName(String plantName) {
//     if (WiFi.status() != WL_CONNECTED) {
//         Serial.println("WiFi not connected!");
//         return -1;
//     }

//     HTTPClient http;
//     String url = "https://perenual.com/api/v2/species-list?key" + String(PLANT_API_KEY) + "&q=" + urlEncode(plantName);
//     http.begin(url);
//     int httpResponseCode = http.GET();
//     if (httpResponseCode == 200) {
//         String payload = http.getString();
//         DynamicJsonDocument doc(1024);
//         DeserializationError error = deserializeJson(doc, payload);
//         if (!error) {
//             int plantId = doc["data"][0]["id"];
//             http.end();
//             return plantId;
//         } else {
//             Serial.println("JSON deserialization failed");
//         }
//     } else {
//         Serial.print("HTTP GET failed, error: ");
//         Serial.println(httpResponseCode);
//     }
// }

void sendTelegramMessage(String message) {
  Serial.print("Sending message: ");
  Serial.println(message);
  
  // 1. Send the message
  // Syntax: bot.sendMessage(chat_id, text, parse_mode)
  bool success = bot.sendMessage(CHAT_ID, message, "");

  if (success) {
    Serial.println("✅ Message sent successfully!");
  } else {
    Serial.println("❌ Failed to send. Check WiFi or Token.");
  }
}