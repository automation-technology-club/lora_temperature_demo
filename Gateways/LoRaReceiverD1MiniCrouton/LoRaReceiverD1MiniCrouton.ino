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

/*
 * http://crouton.mybluemix.net/crouton/dashboard
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
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
/* Change MQTT_MAX_PACKET_SIZE in PubSubClient.h to 512 */

#define SS      D4 
#define RST     22 
#define DI0     D8 
#define OLED_RESET 0  // GPIO0


/*
 *  "{\"deviceInfo\":{\"name\":\"Temperatures\",\"endPoints\":{\"t\":{\"title\":\"Node 4 Temperature\",\"card-type\":\"crouton-simple-text\",\"units\":\"F\",\"values\":{\"value\":0}},\"h\":{\"title\":\"Node 4 Humidity\",\"card-type\":\"crouton-simple-text\",\"units\":\"\",\"values\":{\"value\":0}}},\"description\":\"Temperatures from LoRa Nodes\",\"status\":\"ok\"}}"};
 */
 
const char* deviceInfo[] = {"{\"deviceInfo\":{\"name\":\"Temperature\",\"endPoints\":{\"t\":{\"title\":\"Node 1\",\"card-type\":\"crouton-simple-text\",\"units\":\"F\",\"values\":{\"value\":0}},\"h\":{\"title\":\"Node 1 Humidity\",\"card-type\":\"crouton-simple-text\",\"units\":\"\",\"values\":{\"value\":0}}},\"description\":\"LoRa Node\",\"status\":\"ok\"}}",
 "{\"deviceInfo\":{\"name\":\"Temperature\",\"endPoints\":{\"t\":{\"title\":\"Node 2\",\"card-type\":\"crouton-simple-text\",\"units\":\"F\",\"values\":{\"value\":0}},\"h\":{\"title\":\"Node 2 Humidity\",\"card-type\":\"crouton-simple-text\",\"units\":\"\",\"values\":{\"value\":0}}},\"description\":\"LoRa Node\",\"status\":\"ok\"}}",
 "{\"deviceInfo\":{\"name\":\"Temperature\",\"endPoints\":{\"t\":{\"title\":\"Node 3\",\"card-type\":\"crouton-simple-text\",\"units\":\"F\",\"values\":{\"value\":0}},\"h\":{\"title\":\"Node 3 Humidity\",\"card-type\":\"crouton-simple-text\",\"units\":\"\",\"values\":{\"value\":0}}},\"description\":\"LoRa Node\",\"status\":\"ok\"}}",
 "{\"deviceInfo\":{\"name\":\"Temperature\",\"endPoints\":{\"t\":{\"title\":\"Node 4\",\"card-type\":\"crouton-simple-text\",\"units\":\"F\",\"values\":{\"value\":0}},\"h\":{\"title\":\"Node 4 Humidity\",\"card-type\":\"crouton-simple-text\",\"units\":\"\",\"values\":{\"value\":0}}},\"description\":\"LoRa Node\",\"status\":\"ok\"}}"};
 
const char* mqtt_server = "broker.hivemq.com";

const char* names[] = {"/outbox/node1LFMIOT/deviceInfo","/outbox/node2LFMIOT/deviceInfo","/outbox/node3LFMIOT/deviceInfo","/outbox/node4LFMIOT/deviceInfo"};
const char* nodet[] = {"NULL","/outbox/node1LFMIOT/t","/outbox/node2LFMIOT/t","/outbox/node3LFMIOT/t","/outbox/node4LFMIOT/t"};
const char* nodeh[] = {"NULL","/outbox/node1LFMIOT/h","/outbox/node2LFMIOT/h","/outbox/node3LFMIOT/h","/outbox/node4LFMIOT/h"};

Adafruit_D1_SSD1306 display(OLED_RESET);
WiFiClient  espClient;
PubSubClient client(espClient);

int n; //node number
float t, h; //tempature and humidity

void setup() {
  Serial.begin(9600);
  //while (!Serial);
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
  client.setServer(mqtt_server, 1883);
reconnect();
delay(1000);
updateDeviceInfo();

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
if (!client.connected()) {
    reconnect();
  }
  client.loop();

updateOLED();
updateCrouton();
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

void updateDeviceInfo() {
  for (int i=0; i<4; i++) {
  client.publish(names[i], deviceInfo[i]);
  //Serial.println(names[i]);
  Serial.println(deviceInfo[i]);
  delay(1500);
  }
}

void updateCrouton() {
  char tBuff[20];
  char hBuff[20];
  sprintf(tBuff, "{\"value\":%d.%02d}",(int)t,(int)(t * 100) %100);
  sprintf(hBuff, "{\"value\":%d.%02d}",(int)h,(int)(h * 100) %100);
  client.publish(nodet[n], tBuff);
  //delay(500);
  client.publish(nodeh[n], hBuff);
  //delay(500);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client165648844654")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      
      //client.publish("/outbox/robot/deviceInfo", deviceInfo);
      // ... and resubscribe
      //client.subscribe("/inbox/robot/deviceInfo");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
  /*
 * mosquitto_pub -h broker.hivemq.com -t /outbox/robot/deviceInfo -m '{"deviceInfo":{"name":"Temperatures","endPoints":{"1t":{"title":"Node 1 Temperature","card-type":"crouton-simple-text","units":"F","values":{"value":0}},"2t":{"title":"Node 2 Temperature","card-type":"crouton-simple-text","units":"F","values":{"value":0}},"3t":{"title":"Node 3 Temperature","card-type":"crouton-simple-text","units":"F","values":{"value":0}},"4t":{"title":"Node 4 Temperature","card-type":"crouton-simple-text","units":"F","values":{"value":0}},"1h":{"title":"Node 1 Humidity","card-type":"crouton-simple-text","units":"","values":{"value":0}},"2h":{"title":"Node 2 Humidity","card-type":"crouton-simple-text","units":"","values":{"value":0}},"3h":{"title":"Node 3 Humidity","card-type":"crouton-simple-text","units":"","values":{"value":0}},"4h":{"title":"Node 4 Humidity","card-type":"crouton-simple-text","units":"","values":{"value":0}}},"description":"Temperatures from LoRa Nodes","status":"ok"}}'
 * 

 */
