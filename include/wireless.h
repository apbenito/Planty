#pragma once
#include <Arduino.h>
#include <WiFi.h> // or other wireless library as needed
#include <HTTPClient.h>

// Placeholder wireless interface for future implementation
void wireless_begin();
void wireless_send(const String &payload);
