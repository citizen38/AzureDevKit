#include <AZ3166WiFi.h>
#include "MQTTClient.h"
#include "MQTTNetwork.h"
#include "Telemetry.h"

char printbuf[128];
bool hasWifi = false;
int arrivedcount = 0;
char rxmessage[128];



void messageArrived(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;
  
  sprintf(printbuf, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n", 
    ++arrivedcount, message.qos, message.retained, message.dup, message.id);
  Serial.println(printbuf);
  sprintf(rxmessage, (char*)message.payload);
  Serial.println(message.payloadlen);
  rxmessage[message.payloadlen]='\0'; 
    
  }

MQTTNetwork mqttNetwork;
MQTT::Client<MQTTNetwork, Countdown> client = MQTT::Client<MQTTNetwork, Countdown>(mqttNetwork);

byte mac[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };  // replace with your device's MAC
const char* topic = "IBMMQTTTest";

void connect()
{
  char hostname[] = "192.168.0.111";
  int port = 1883;
  sprintf(printbuf, "Connecting to %s:%d\n", hostname, port);
  Serial.print(printbuf);
  int rc = mqttNetwork.connect(hostname, port);
  if (rc != 1)
  {
    sprintf(printbuf, "rc from TCP connect is %d\n", rc);
    Serial.print(printbuf);
  }

  Serial.println("MQTT connecting");
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
  data.MQTTVersion = 3;
  data.clientID.cstring = (char*)"IBMMQTTTest";
  rc = client.connect(data);
  if (rc != 0)
  {
    sprintf(printbuf, "rc from MQTT connect is %d\n", rc);
    Serial.print(printbuf);
  }
  Serial.println("MQTT connected");
  
  rc = client.subscribe(topic, MQTT::QOS2, messageArrived);   
  if (rc != 0)
  {
    sprintf(printbuf, "rc from MQTT subscribe is %d\n", rc);
    Serial.print(printbuf);
  }
  Serial.println("MQTT subscribed");
}

void initWifi()
{
  Screen.print("MQTT TEST\r\n \r\nConnecting...\r\n");

  if (WiFi.begin() == WL_CONNECTED)
  {
    IPAddress ip = WiFi.localIP();
    Screen.print(1, ip.get_address());
    hasWifi = true;
    Screen.print(2, "Running... \r\n");
  }
  else
  {
    Screen.print(1, "No Wi-Fi\r\n ");
}
}

void setup()
{
//Initialize serial and Wi-Fi:
  Serial.begin(115200);
  initWifi();
}

void loop()
{
  if (!client.isConnected())
    connect();
  
  MQTT::Message message;
  client.yield(1000);

  Screen.print(2, rxmessage);

  Serial.println("loop");
  delay(2000);
}
