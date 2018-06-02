#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

const char *ssid     = "test";
const char *password = "19960101";
const char *host = "192.168.191.1";
const char *addr_o = "192.168.191.1";

unsigned int tcpPort = 8081;
unsigned int udpPort = 1001;
unsigned int port_o = 1002;
WiFiUDP udp;
WiFiClient client;

byte pre = 0, next = 0, ID = 2, function = 1;
byte pre_o = 9, next_o = 9, ID_o = 1, function_o = 2;
String MessSend = "", MessRece = "", udpMess = "";
byte  dest_n = 0, dest_s = 0, status = 0;
byte dest[10] = {9, 9, 9, 9, 9, 9, 9, 9, 9};

void setup()
{
  Serial.begin(115200);
  delay(2000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  while (!client.connected())
  {
    if (!client.connect(host, tcpPort))
    {
      Serial.println("connection....");
      //client.stop();
      delay(500);
    }
  }
  Serial.println("Starting UDP");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  udp.begin(udpPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
}


void loop()
{
  while (udp.parsePacket()) {
    udpMess = "";
    Serial.print("packet received");
    udp.read();
    for (int j = 1; j < 5; j++)
    {
      char Mess = udp.read();
      udpMess = udpMess + Mess;
    }
    Serial.println(udpMess);
    MessDe_o();
  }
  while (client.available()) {
    char k = client.read();
    dest[dest_n] = byte(k) - 48;
    dest_n += 1;
  }
  switch (byte(Serial.read())) {
    case 43:
      MessRece = Serial.parseInt();
      MessDe();
      Mess();
      break;
    case 35:
      MessRece = Serial.parseInt();
      MessSend = "";
      MessSend = "#" + MessRece;
      client.println(MessSend);
      udp.beginPacket(addr_o, port_o);
      for (int i = 0; i < 4; i++)
        udp.write(MessRece[i]);
      udp.endPacket();
      break;
  }
}
void Mess()
{
  switch (function) {
    case 1: MessSd(); break;
    case 2: MessSd_o(); break;
    case 0: status = 0; break;
  }
}
void MessDe()
{
  ID = byte(MessRece[0]) - 48;
  function = byte(MessRece[1]) - 48;
  pre = byte(MessRece[2]) - 48;
  next = byte(MessRece[3]) - 48;
}
void MessDe_o()
{
  ID_o = byte(udpMess[0]) - 48;
  function_o = byte(udpMess[1]) - 48;
  pre_o = byte(udpMess[2]) - 48;
  next_o = byte(udpMess[3]) - 48;
}
void MessSd()
{
  MessSend = "";
  MessSend = "-" + String(ID) + String(function) + String(pre) + String(dest[dest_s]);
  if (dest[dest_s] != 9)
    dest_s += 1;
  Serial.println(MessSend);
}
void MessSd_o()
{
  MessSend = "";
  MessSend = "-" + String(ID_o) + String(function_o) + String(pre_o) + String(next_o);
  Serial.println(MessSend);
}
