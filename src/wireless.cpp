#include "wireless.h"
#include <Arduino.h>
#include <WiFi.h> // or other wireless library as needed
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "GordasUnidas 2.4 GHz";         // replace with your network SSID (name)
const char* password = "tontoelquelolea"; // replace with your network password
const char* discordWebhook = "https://discordapp.com/api/webhooks/1446217579881566373/2b4bFcqg7l03gnMUi2HWXoTU3_tODeJc1SX5wv4NEi5PubYDOm0d4GcQRu1R2vzcWckg"; // replace with your server URL
void wireless_begin() {

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
}

void wireless_send(const String &payload) {
    if (WiFi.status() == WL_CONNECTED) {
        // Placeholder for sending data
        Serial.print("Sending payload: ");
        Serial.println(payload);

        // Actual sending code would go here (e.g., HTTP POST, MQTT publish, etc.)
        WiFiClientSecure client;
        client.setInsecure(); // Disable certificate verification for simplicity; not recommended for production

        HTTPClient https;

        if (https.begin(client, discordWebhook)) {  // HTTPS
            https.addHeader("Content-Type", "application/json");
        } else {
            Serial.println("Unable to connect");
            return;
        }

        // Discord webhooks expect a JSON body like: {"content": "message"}
        String jsonPayload = String("{\"content\": \"") + payload + String("\"}");
        int httpResponseCode = https.POST(jsonPayload);

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            // String response = https.getString();
            // Serial.print("Response body: ");
            // Serial.println(response);
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }
        https.end();
    } else {
        Serial.println("WiFi not connected. Cannot send payload.");
    }
}
