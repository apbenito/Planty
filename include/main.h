#include <cstdint>

// Analog pins
#define MOIST_PIN_CALATHEA 34
#define MOIST_PIN_DIEFFENBACHIA 35
#define TEMP_PIN 32 // ADC1 pin (GPIO32)

// E-Ink config
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