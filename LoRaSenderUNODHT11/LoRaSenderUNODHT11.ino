/* LeRoy Miller (C) 2018, see license.h for license information */
 
#include <SPI.h>
#include <LoRa.h> //https://github.com/sandeepmistry/arduino-LoRa
#include <Adafruit_Sensor.h> //https://github.com/adafruit/Adafruit_Sensor
#include <DHT.h> //https://github.com/adafruit/DHT-sensor-library
#include <DHT_U.h>

#define NODE 3 //Node Number of Sensor
#define DHTPIN            6         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 

float t, h; //tempature and humidity
int counter;
uint32_t delayMS;

DHT_Unified dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(9600);
  //while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 dht.begin();
 sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void loop() {
  getTemp();
  sendPacket();
  delay(15000);
}



void getTemp() {
delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  
  t = event.temperature*1.8+32;
  dht.humidity().getEvent(&event);
  h = event.relative_humidity;

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
