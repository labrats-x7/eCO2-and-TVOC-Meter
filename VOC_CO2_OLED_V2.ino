#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "Adafruit_CCS811.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin) (not used)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_CCS811 ccs;


//------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("CCS811 + OLED");

  //Setup OLED--------------------------

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(6,13);
  display.println(F("CO2 & TVOC"));
  display.setCursor(35,40);
  display.println(F("Meter"));
    // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(5000);
  fillroundrect();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(BLACK);        // Draw white text
  display.setCursor(48,28);
  display.print(F("by x-7"));
  display.display();
  delay(5000);


  //Setup CS811-------------------------

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }

  //calibrate temperature sensor
  while(!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);

}

void loop() {
  
    if(ccs.available()){
    float temp = ccs.calculateTemperature();
    if(!ccs.readData()){
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm, TVOC: ");
      Serial.print(ccs.getTVOC());
      Serial.print("ppb,   Temp:");
      Serial.println(temp);
      display.clearDisplay();
      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(WHITE);        // Draw white text
      display.setCursor(13,0);
      display.print(F("eCO2 & TVOC Meter"));
      display.setCursor(0,12);
      display.print(F("Quali:"));
      display.setTextSize(2);
      if(ccs.geteCO2()<800){
        display.print(F("gut"));
      }
      if(ccs.geteCO2()>=800 && ccs.geteCO2()<1000){
        display.print(F("mittel"));
      }
      if(ccs.geteCO2()>=1000 && ccs.geteCO2()<1400){
        display.print(F("minder"));
      }
      if(ccs.geteCO2()>=1400 && ccs.geteCO2()<2400){
        display.print(F("lausig"));
      }
      if(ccs.geteCO2()>=2400 && ccs.geteCO2()<3500){
        display.print(F("ACHTUNG"));
      }  
      if(ccs.geteCO2()>=3500){
        display.print(F("GEFAHR!"));
      } 
      display.setTextSize(1);
      display.setCursor(0,31);           
      display.print(F("eCO2: "));
      display.setTextSize(2);
      display.print(ccs.geteCO2());
      display.println(F("ppm"));
      display.setTextSize(1);
      display.setCursor(0,49);           
      display.print(F("TVOC: "));
      display.setTextSize(2);
      display.print(ccs.getTVOC());
      display.println(F("ppb"));
      if(ccs.geteCO2()>1400){
        display.invertDisplay(true);
      }
      else{
        display.invertDisplay(false);
      }
      display.display();
    }
    
    else{
      Serial.println("ERROR!");
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(10,20);
      display.println(F("ERROR"));
      while(1);
    }
  delay(500);
  }
}

void fillroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, INVERSE);
    display.display();
    delay(1);
  }
}
