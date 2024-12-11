/* Turn on/off lamps for different periods
 */
// Объем памяти EEPROM = 4096 Б ? In Arduino you call EEPROM.begin(), but in ESP8266 you have to call EEPROM.begin(n), where n is the total number of bytes you will need.

#include <Arduino.h>
#include <ESP8266WiFi.h>             // OTA etc
#include <ESP8266WebServer.h>        // OTA
#include <ESP8266HTTPUpdateServer.h> //OTA
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#include <browser.cpp>

// SETTINGS
const bool OTAEnabled = true; // OTA
const bool browserEnabled = true; // Включение выключение сервера (для удобства скрытия блоков кода)
const bool wireEnabled = false;   //(для удобства скрытия блоков кода)

byte typeLightCntrl = 1;
// 1 - 3box
// 2 - dali const and parabolic illuminance

// Default
bool showResults = false;
bool lightOn = false;
bool startMeasure = false;

// define pins
const byte relayPin[] = {15, 12, 13, 2}; //! PWM //{15, 12, 13, 2}
// контакт GPIO2 на ESP8266  - cвязан со светодиодом, который на плате
/// const byte lightPinMRGB[] = {15, 12, 13, 2}; //пин 0 - выход ШИМ первого светильника

// для данного эксперимента принимаем, что сутки длятся 48 часов (для удобства программирования)
// Задаем длительность каждого периода включенного и выключенного состояния (первая цифра - часы, вторая цифра - минуты)
bool lampStatus[4]; // состояние ламп
const int timeSets[4][4][4] = {
    // ч, м вкл, ч, м выкл //еще одна //еще одна // еще одна
    // Принимаем продолжительность светлого 14 часов, темного 10 часов в сутки
    {{0, 7, 21, 00}, {24 + 7, 0, 24 + 21, 0}, {-1, -1, -1, -1}, {-1, -1, -1, -1}}, // 14+14
    {{0, 7, 21, 00}, {24 + 7, 0, 24 + 21, 0}, {-1, -1, -1, -1}, {-1, -1, -1, -1}}, // 14+14
    {{0, 7, 21, 00}, {24 + 7, 0, 24 + 21, 0}, {-1, -1, -1, -1}, {-1, -1, -1, -1}}, // 14+14
    {{-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}},
};
/*Для первого эксперимента было:
{{19, 0, 47, 00}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}},
    {{9, 0, 23, 00}, {33, 0, 47, 0}, {-1, -1, -1, -1}, {-1, -1, -1, -1}},
    {{0, 0, 6, 0}, {11, 0, 18, 0}, {23, 0, 30, 0}, {35, 0, 42, 0}}, //6+7+7+7 - похоже ошибка была
    {{-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}},
*/

// uint8_t i2c_rcv = 255; // data received from I2C bus
// SoftwareSerial arduinoSerial(15, 13); //GPIO15 (TX) and GPIO13 (RX)

const char *ssid = "RT-GPON-37C8";   //"GonioRad2.4G";//"DIR-615T-74CC"; // ssid = "ROSTELECOM_CB50";  "DIR-615T-74CC"    "M-Redmi"
const char *password = "Bgnagz3aL1"; //"astz2023";//"44220846";    // password = "734GFTCN";      "44220846"  "Moward-WiFi"

// OTA
#define OTAUSER "admin"                  // OTA Логин для входа в OTA
#define OTAPASSWORD "admin"              // OTA Пароль для входа в ОТА
#define OTAPATH "/firmware"              // OTA Путь, который будем дописывать после ip адреса в браузере.
#define SERVERPORT 81                    // OTA Порт для входа, он стандартный 80 это порт http (изменил на 81, был конфликт адресов)
ESP8266WebServer HttpServer(SERVERPORT); // OTA
ESP8266HTTPUpdateServer httpUpdater;     // OTA

//=============Обновление даты времени=============
#include <NTPClient.h>
#include <WiFiUdp.h>
#define NTP_OFFSET 3 * 60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000      // In miliseconds
#define NTP_ADDRESS "time.nist.gov" // time.nist.gov, europe.pool.ntp.org
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
//=================================================

WiFiServer server(80); //  порт веб-сервера

void Browser(bool lampPosition[4], String formattedTime)
{
  Serial.println(" Вывод информации браузера");
  unsigned int timer = millis();
  WiFiClient client = server.available();
  if (!client)
  {
    if (millis() > timer + 11 * 1000) // если клиент не отвалился через минуту
    {
      Serial.println("timer 11 s");
      ESP.restart(); // перегружаемся
    }
    return;
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  не забываем это...
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println(" <head>");
  client.println("  <meta charset=\"utf-8\">");
  client.println("  <title>Гроубокс</title>");
  client.println(" </head>");

  client.println(" <body>");

  client.println("IP: ");
  client.print(WiFi.localIP());
  client.println("<br>");

  // Вывод текущих значений освещенности
  client.println("Now " + formattedTime + "<br>");
 for (int i = 0; i < 3; i++)
  {
    client.println("box" + String(i + 1) + " = " + String(!lampPosition[i]) + "<br>");
  }

  client.println("<br>");
  client.println("Click <a href=\":81/firmware\"> here to upload firmware </a> <br>");
  client.println("</body>");
  client.println("</html>");
}
void handleNotFound() // OTA
{
  HttpServer.send(404, "text/plain", "404: Not found... <a href=\"/firmware\">firmware</a> <br>");
}
//               :81/firmware
void setup()
{
  timeClient.begin(); // NTPClient
  Serial.begin(9600);
  while (!Serial)
    delay(99); // ждем открытия монитора порта в IDE Arduino

  // подключаемся к WiFi-сети:
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

  // запускаем сервер:
  server.begin();
  Serial.println("Server started");
  // печатаем IP-адрес:
  Serial.print("Use this URL to connect: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  switch (typeLightCntrl)
  {
  case 1:
    for (int i = 0; i < 4; i++)
    {
      pinMode(relayPin[i], OUTPUT);
    }
    break;

  case 2:
    break;
  }
  delay(1);

  if (OTAEnabled)
  {
    httpUpdater.setup(&HttpServer, OTAPATH, OTAUSER, OTAPASSWORD);
    HttpServer.onNotFound(handleNotFound);
    HttpServer.begin();
  }
}

void loop()
{

  timeClient.update(); // Обновление даты времени=============
  unsigned long currentTime = timeClient.getEpochTime();

  Serial.println(timeClient.getFormattedTime());
  int currrentMin = (currentTime % 172800) / 60;
  Serial.println(currrentMin);

  switch (typeLightCntrl)
  {
  case 1:
    for (byte i = 0; i < 4; i++) // перебор ламп (боксков)
    {
      bool lampOn = false;
      Serial.println("Бокс" + String(i) + ": ");

      for (byte j = 0; j < 4; j++) // Перебор временных отрезков (заложено, что максимум четыре возможно)
      {
        int onT = timeSets[i][j][0] * 60 + timeSets[i][j][1];  // время включения
        int offT = timeSets[i][j][2] * 60 + timeSets[i][j][3]; // время выключния
        Serial.print(" :: Период" + String(j) + ": OnT=" + String(onT) + " offT=" + String(offT));
        if ((currrentMin > onT) & (currrentMin < offT))
          lampOn = true;
      }

      if (!lampOn)
      {
        digitalWrite(relayPin[i], HIGH);
        lampStatus[i] = true;
        Serial.println(" :::: Вкл");
      }
      else
      {
        digitalWrite(relayPin[i], LOW);
        lampStatus[i] = false;
        Serial.println(" :::: Откл");
      }
    }
break;
  case 2:
    break;
  }

  if (OTAEnabled)
  {
    HttpServer.handleClient(); // Прослушивание HTTP-запросов от клиентов
  }
  delay(1 * 1000);

  Browser(lampStatus, timeClient.getFormattedTime());
}