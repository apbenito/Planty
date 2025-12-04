#include "display.h"
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define EPD_SS 3
#define EPD_DC 5
#define EPD_RST 17
#define EPD_BUSY 16

GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(EPD_SS, EPD_DC, EPD_RST, EPD_BUSY));

#define TEXT_LINES 3
uint16_t TextCursors[2][TEXT_LINES];
const char* MSG[TEXT_LINES] = {
  "E-Ink/E-Paper Display",
  "Arduino Tutorial From",
  "0"
};

void display_begin() {
  display.init();
}

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
