/*
 * LAB Name: Arduino 2.9" E-Paper Display (GxEPD2 Library) - Text Example
 * Author: Khaled Magdy
 * For More Info Visit: www.DeepBlueMbedded.com
*/
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
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

#define MOIST_PIN 34
#define TEMP_PIN 32 // Use ADC1 pin (GPIO32) for more reliable ADC readings on ESP32
#define R_FIXED 10000
#define VCC 3.3

const float A = 8.202819e-04;
const float B = 2.636000e-04;
const float C = 1.360785e-07;
const int ADC_MAX = 4095;

GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(EPD_SS, EPD_DC, EPD_RST, EPD_BUSY));

uint16_t TextCursors[2][TEXT_LINES];
const char* MSG[TEXT_LINES] = {
  "E-Ink/E-Paper Display",
  "Arduino Tutorial From",
  "0"
};
void PrintMessages(int light, int humidity, float temperature);
void WhiteScreen();

 
void PrintMessages(int light, int humidity, float temperature)
{
  int16_t tbx, tby; 
  uint16_t tbw, tbh;
  display.setRotation(3);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  for(int i=0; i<TEXT_LINES; i++)
  {
    display.getTextBounds(MSG[i], 0, 0, &tbx, &tby, &tbw, &tbh);
    TextCursors[0][i] = (display.width() - tbw) / 2;
    TextCursors[1][i] = ((display.height()*(i+1)) /(TEXT_LINES+1)) + (tbh/2);
  }
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println("light: ");
    display.println("humidity: ");
    display.println("temperature: ");
    display.setCursor(150, 20);  
    display.println(light);
    display.setCursor(150, 38);  
    display.println(humidity);
    display.setCursor(150, 56);  
    display.println(temperature);
  }
  while(display.nextPage());
}

// void WhiteScreen(){
//   display.setFullWindow();
//   display.firstPage();
//   do{
//       display.fillScreen(GxEPD_WHITE);
//   }
//     while(display.nextPage());
// }

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

bool read_am2320_i2c(float &humidity, float &temperature) {
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



// Initialize VEML7700 with default config (you can change gain/IT here)
bool init_veml7700() {
    Wire.beginTransmission(VEML7700_ADDR);
    // Default config recommended: 0x0000 (gain = 1, integration 100ms, persistence 1, enabled)
    uint16_t conf = 0x0000;
    Wire.write(VEML7700_REG_CONF);
    Wire.write(conf & 0xFF);        // LSB
    Wire.write((conf >> 8) & 0xFF); // MSB
    return (Wire.endTransmission() == 0);
}

// Read raw ALS register (16-bit, LSB then MSB)
bool read_veml7700_raw(uint16_t &als_raw) {
    Wire.beginTransmission(VEML7700_ADDR);
    Wire.write(VEML7700_REG_ALS);
    if (Wire.endTransmission(false) != 0) return false; // send restart
    uint8_t n = Wire.requestFrom(VEML7700_ADDR, (uint8_t)2);
    if (n != 2) return false;
    uint8_t l = Wire.read();
    uint8_t h = Wire.read();
    als_raw = (uint16_t)h << 8 | l;
    return true;
}

// Optional helper to read and (approx.) convert to lux — conversion depends on config.
// Here we only return raw value; implement lux conversion per datasheet or use a library.
bool read_veml7700_lux(float &lux_out) {
    uint16_t raw;
    if (!read_veml7700_raw(raw)) return false;
    // No conversion performed here. If you want an approximate lux conversion,
    // apply the datasheet formula depending on gain and integration time.
    lux_out = (float)raw;
    return true;
}

// Read 16-bit register (LSB then MSB)
bool read16reg(uint8_t addr, uint8_t reg, uint16_t &out) {
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

void veml_diagnostics() {
  uint16_t conf;
  Serial.println("--- VEML diagnostics ---");
  if (read16reg(VEML7700_ADDR, VEML7700_REG_CONF, conf)) {
    Serial.print("Config reg = 0x"); Serial.println(conf, HEX);
  } else {
    Serial.println("Failed to read config reg");
  }
  uint16_t als;
  if (read16reg(VEML7700_ADDR, VEML7700_REG_ALS, als)) {
    Serial.print("ALS raw = 0x"); Serial.println(als, HEX);
    Serial.print("ALS raw (dec) = "); Serial.println(als);
  } else {
    Serial.println("Failed to read ALS reg");
  }

  // If config is zero or ALS is zero, try writing a safe config:
  // Safe config: enable sensor, gain=1, integration ~100ms — value depends on bits.
  // We'll try writing 0x0000 first (common default), then 0x00C0 as an attempt.
  // Note: if your breakout has different defaults, adjust as needed.
  Serial.println("Writing test config 0x0000...");
  if (write16reg(VEML7700_ADDR, VEML7700_REG_CONF, 0x0000)) {
    Serial.println("Write ok, waiting 50ms...");
    delay(50);
    if (read16reg(VEML7700_ADDR, VEML7700_REG_ALS, als)) {
      Serial.print("ALS after write = "); Serial.println(als);
    }
  } else {
    Serial.println("Config write failed");
  }
}

float calculateTempSteinhart(float r) {
  // Safety check to prevent log of 0 or negative numbers
  if (r <= 0) return -999.0; 

  float logR = log(r);
  float inverseT = A + B * logR + C * pow(logR, 3);
  
  // Convert Kelvin to Celsius
  return (1.0 / inverseT) - 273.15;
}

void setup() {
  Serial.begin(9600);
  Wire.begin(21, 22); // SDA = GPIO21, SCL = GPIO22 (ESP32 defaults)
  delay(2000);
  
  // Initialize VEML7700
  if (init_veml7700()) {
    Serial.println("VEML7700 initialized");
  } else {
    Serial.println("VEML7700 init failed");
  }

  delay(100);
  veml_diagnostics();
  display.init();

  pinMode(MOIST_PIN, INPUT);
  pinMode(TEMP_PIN, INPUT);
  // Ensure ADC attenuation maps full 0-3.3V range for this pin
  analogSetPinAttenuation(TEMP_PIN, ADC_11db);
}

void loop() {
  // float h = 0.0f, t = 0.0f;
  
  // if (read_am2320_i2c(h, t)) {
  //     Serial.print("Humidity: "); Serial.print(h); Serial.print(" %\t");
  //     Serial.print("Temp: "); Serial.print(t); Serial.println(" °C");
  // } else {
  //     Serial.println("AM2320 read failed");
  // }

  // Read VEML7700
  uint16_t als_raw;
  if (read_veml7700_raw(als_raw)) {
      Serial.print("VEML7700 raw ALS: "); Serial.println((float)als_raw);
    } else {
      Serial.println("VEML7700 read failed");
    }
    
    int moistureValue = analogRead(MOIST_PIN);
    Serial.print("Soil moisture: ");
    Serial.println(moistureValue);

    int adcVal = analogRead(TEMP_PIN);
    Serial.print("ADC raw temp: "); Serial.println(adcVal);
    float tempC = -999.0;
    if (adcVal <= 0 || adcVal >= ADC_MAX) {
      Serial.println("ADC reading out of range (0 or max). Check wiring/pin selection.");
    } else {
      float resistance = R_FIXED * ((float)adcVal / (ADC_MAX - adcVal));
      Serial.print("Calculated resistance (ohms): "); Serial.println(resistance);
      tempC = calculateTempSteinhart(resistance);
      Serial.print("Temperature sensor value: ");
      Serial.println(tempC);
    }
    
  PrintMessages(als_raw, moistureValue, tempC);
  
  delay(2000); // AM2302 sampling rate is once every 2 seconds
}