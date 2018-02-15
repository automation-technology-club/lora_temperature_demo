/* LeRoy Miller (C) 2018, see license.h for license information */

/*
 *  WeMos D1          RFM9x Module
  GPIO12 (D6) <----> MISO
  GPIO13 (D7) <----> MOSI
  GPIO14 (D5) <----> CLK
  GPIO15 (D8) <----> DIO0/D2 OR DIO1/D3 OR DIO2/D4
  GPIO02 (D4) <----> SEL (Chip Select)
  RST         <----> RST
  * Appears D1, D2, D3, A0, and D0 free
  */
  
#include <SPI.h>
#include <LoRa.h> //https://github.com/sandeepmistry/arduino-LoRa
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_D1_SSD1306.h> 
/*
 * https://github.com/kd8bxp/mcauser-64x48-OLED-SSD1306-library
 *Slightly modified version of this library 
 *https://github.com/mcauser/Adafruit_SSD1306/tree/esp8266-64x48
 *
 */
#include <WEMOS_SHT3X.h> //https://github.com/wemos/WEMOS_SHT3x_Arduino_Library

#define NODE 1 //Node Number of Sensor
#define SS      D4 
#define RST     22 
#define DI0     D8 
#define OLED_RESET 0  // GPIO0
Adafruit_D1_SSD1306 display(OLED_RESET);

SHT3X sht30(0x45);

float t, h; //tempature and humidity
int counter;

void setup() {
  Serial.begin(9600);
  while (!Serial);
LoRa.setPins(SS,RST,DI0);
  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop() {
  getTemp();
  updateOLED();
  sendPacket();
  yield();
  delay(15000);
  //ESP.deepSleep(15*1000000);
}

void updateOLED() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.println("T: ");
  display.setTextSize(2);
  display.println(t);
  display.setTextSize(1);
  display.println("H: ");
  display.setTextSize(2);
  display.println(h);
  display.display();
  
}

void getTemp() {

if (sht30.get() == 0) {
  t = sht30.fTemp;
  h = sht30.humidity;
} else {
  Serial.println("Error!");
}
}
void sendPacket() {
  Serial.print("Sending packet: ");
  char hex[20];
  sprintf(hex, "%d %d.%02d %d.%02d",NODE,(int)t,(int)(t*100)%100,(int)h,(int)(h*100)%100);
  //sprintf(hex, "%d.%02d %d.%02d",(int)t,(int)(t*100)%100,(int)h,(int)(h*100)%100);
  //sprintf(hex, "%02d %02d",(int)t,(int)h);
  
  // send packet
  LoRa.beginPacket();
  LoRa.print((String)hex);
  LoRa.endPacket();
  delay(5000);
}
