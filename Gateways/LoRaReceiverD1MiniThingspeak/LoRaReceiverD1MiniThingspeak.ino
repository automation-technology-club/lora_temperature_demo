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
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_D1_SSD1306.h>
/*
 * https://github.com/kd8bxp/mcauser-64x48-OLED-SSD1306-library
 *Slightly modified version of this library 
 *https://github.com/mcauser/Adafruit_SSD1306/tree/esp8266-64x48
 *
 */
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include "ThingSpeak.h" //https://github.com/mathworks/thingspeak-arduino

#define SS      D4 
#define RST     22 
#define DI0     D8 
#define OLED_RESET 0  // GPIO0
Adafruit_D1_SSD1306 display(OLED_RESET);
WiFiClient  client;

int n; //node number
float t, h; //tempature and humidity
unsigned long myChannelNumber = 123;
const char * myWriteAPIKey = "";

void setup() {
  Serial.begin(9600);
  while (!Serial);
WiFiManager wifiManager;
wifiManager.autoConnect("AutoConnectAP");
  Serial.println("LoRa Receiver");
 LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  ThingSpeak.begin(client);
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
   

String buffer;
    // read packet
    while (LoRa.available()) {
      //Serial.print((char)LoRa.read());
      buffer = buffer+(char)LoRa.read();
    }
    Serial.println(buffer);

n = buffer.toInt();
buffer.remove(0,2);
t = buffer.toFloat();
buffer.remove(0,6);
h = buffer.toFloat();
updateOLED();
updateThingSpeak();
    Serial.print("Node: "); Serial.print(n); Serial.print(" T: ");Serial.print(t); Serial.print(" H: ");Serial.println(h);
    yield();
  }
 
  yield();
}

void updateOLED() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.print("Node #: ");
  display.println(n);
  display.setTextSize(2);
  display.println(t);
  display.setTextSize(1);
  display.println("H: ");
  display.setTextSize(2);
  display.println(h);
  display.display();
  
}

void updateThingSpeak() {
   ThingSpeak.setField(1,t);
  ThingSpeak.setField(2,h);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

