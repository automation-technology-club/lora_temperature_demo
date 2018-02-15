/* LeRoy Miller (C) 2018, see license.h for license information */

/*
 * http://crouton.mybluemix.net/crouton/dashboard
 */

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "";
const char* password = "";

//#define MQTT_MAX_PACKET_SIZE 1024
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

int n;
float t,h;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setup_wifi();
client.setServer(mqtt_server, 1883);
reconnect();
delay(1000);
updateDeviceInfo();

}

void loop() {
  // put your main code here, to run repeatedly:
n = random(1,5);
t = random(35, 101);
h = random(0, 101);
if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //updateDeviceInfo();
  updateCrouton();
  yield();
  delay(1500);
}

void updateDeviceInfo() {
  for (int i=0; i<4; i++) {
  client.publish(names[i], deviceInfo[i]);
  //Serial.println(names[i]);
  Serial.println(deviceInfo[i]);
  delay(1500);
  }
}



/*void updateDeviceInfo() {
  client.publish("/outbox/node1LFMIOT/deviceInfo","{\"deviceInfo\":{\"name\":\"Temperature\",\"endPoints\":{\"t\":{\"title\":\"Node 1\",\"card-type\":\"crouton-simple-text\",\"units\":\"F\",\"values\":{\"value\":0}}},\"description\":\"LoRa Node\",\"status\":\"ok\"}}", 574);
delay(500);
 // client.publish("/outbox/node2LFMIOT/deviceInfo","{\"deviceInfo\":{\"name\":\"Temperature\",\"endPoints\":{\"t\":{\"title\":\"Node 2\",\"card-type\":\"crouton-simple-text\",\"units\":\"F\",\"values\":{\"value\":0}}},\"description\":\"LoRa Node\",\"status\":\"ok\"}}");
delay(500);
 // client.publish("/outbox/node3LFMIOT/deviceInfo","{\"deviceInfo\":{\"name\":\"Temperature\",\"endPoints\":{\"t\":{\"title\":\"Node 3\",\"card-type\":\"crouton-simple-text\",\"units\":\"F\",\"values\":{\"value\":0}}},\"description\":\"LoRa Node\",\"status\":\"ok\"}}");
delay(500);
 // client.publish("/outbox/node4LFMIOT/deviceInfo","{\"deviceInfo\":{\"name\":\"Temperature\",\"endPoints\":{\"t\":{\"title\":\"Node 4\",\"card-type\":\"crouton-simple-text\",\"units\":\"F\",\"values\":{\"value\":0}}},\"description\":\"LoRa Node\",\"status\":\"ok\"}}");

}*/

/*
void updateCrouton() {

char tBuff[20];
char hBuff[20];
sprintf(tBuff, "{\"value\":%d.%02d}",(int)t,(int)(t * 100) %100);
sprintf(hBuff, "{\"value\":%d.%02d}",(int)h,(int)(h * 100) %100);
  switch (n) {
    case 1:
    client.publish("/outbox/robot/1t", tBuff);
    delay(500);
    client.publish("/outbox/robot/1h", hBuff);
    break;
    case 2:
    client.publish("/outbox/robot/2t", tBuff);
    delay(500);
    client.publish("/outbox/robot/2h", hBuff);
    break;
    case 3:
    client.publish("/outbox/robot/3t", tBuff);
    delay(500);
    client.publish("/outbox/robot/3h", hBuff);
    break;
    case 4:
    client.publish("/outbox/robot/4t", tBuff);
    delay(500);
    client.publish("/outbox/robot/4h", hBuff);
    break;
  }
  
}
*/

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

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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
