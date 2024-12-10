#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"

String sheetHumid = "";
String sheetTemp = "";
const char *host = "script.google.com";
const char *GScriptId = "AKfycbyu16SoCQiy4jubb2dB__qTZ-gnyrxcGZEzCB9rcTMMkLk8Tnx-8L0eCpPgyK7r_NNiYQ";
const int httpsPort = 443;
const char *fingerprint = "";
//client.connect("https://script.google.com/macros/s/AKfycbycBPgyANI1vGEMSpGJaHGwz1YuDC4J_1fd6N1lmcuffN77ZRvg/exec?tag=test&value=-333", 80);//client.connect(host, port);
String url = String("/macros/s/") + GScriptId + "/exec?tag=restart&value=-1";

int timeInterval1 = 5 * 60 * 1000; //ms
unsigned int timeSaved1 = 0;

HTTPSRedirect *client = nullptr;

void send()
{

  Serial.begin(9600);

  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  Serial.print("Connecting to ");
  Serial.println(host);
  bool flag = false;
  for (int i = 0; i < 5; i++)
  {
    int retval = client->connect(host, httpsPort);
    if (retval == 1)
    {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag)
  {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  Serial.println("\nSend to gSheet start information");
  client->GET(String("/macros/s/") + GScriptId + "/exec?tag1=restart&value1=-1", host); // url, host  ++++++++++++++++++++++++++++++++++++++++

  delete client;
  client = nullptr;
}

void sendToGGheet(int ppm1, int ppm2)
{
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  Serial.print("Connecting to ");
  Serial.println(host);
  bool flag = false;
  for (int i = 0; i < 5; i++)
  {
    int retval = client->connect(host, httpsPort);
    if (retval == 1)
    {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag)
  {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  String urlData = String("/macros/s/") + GScriptId + "/exec?tag1=C02_Concentration1_is&value1=" + String(ppm1) +
                   "&tag2=C02_Concentration2_is&value2=" + String(ppm2);
  delay(500);                 //ДЛЯ ВАЙФАЙ
  client->GET(urlData, host); //   ошибка ++++++++++++++++++++++++++++++++++++++++
  delay(500);                 //ДЛЯ ВАЙФАЙ
  delete client;
  client = nullptr;
}

/*
void sendToGGheet(int ppm1,) {
  

  Serial.println(millis());
  timeSaved = (millis() / timeInterval1) * timeInterval1;
  int ppm = 0;

  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");

 
  Serial.print("Connecting to ");
  Serial.println(host);
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  String urlData = String("/macros/s/") + GScriptId + "/exec?tag=C02_Concentration_is&value=" + String(ppm);
  delay(5000);//ДЛЯ ВАЙФАЙ
    client->GET(urlData, host); //   ошибка ++++++++++++++++++++++++++++++++++++++++
  delay(5000);//ДЛЯ ВАЙФАЙ
  delete client;
  client = nullptr;
   while (millis() < timeSaved + timeInterval1) {
    Serial.println(millis());
  }

}
*/