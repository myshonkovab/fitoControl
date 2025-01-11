
#include <Arduino.h>
#include <ESP8266WiFi.h>             // OTA etc
#include <ESP8266WebServer.h>        // OTA
#include <ESP8266HTTPUpdateServer.h> //OTA

#include <browser.cpp>
#include <Dali.h>

// SETTINGS
const bool OTAEnabled = true;     // OTA
const bool browserEnabled = true; // Включение выключение сервера (для удобства скрытия блоков кода)
const bool wireEnabled = false;   //(для удобства скрытия блоков кода)

byte typeLightCntrl = 2;
// 1 - 3box
// 2 - dali const and parabolic illuminance
const int DALI_TX = D1;   // D1, GPIO5
const int DALI_RX_A = A0; // A0

uint8_t adressBlue = 0;
uint8_t adressConstFlux = 1;
uint8_t adressParabFlux = 4;
int maxConstFlux = 200;
int maxParabFlux = 300;
String browserString1;
String browserString2;

// Default
bool showResults = false;
bool lightOn = false;
bool startMeasure = false;

// define pins
const byte relayPin[] = {15, 12, 13, 2}; //! PWM //{15, 12, 13, 2}
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

const int countWiFiAcc = 3;
const char *ssid[countWiFiAcc] = {"DIR-615T-74CC", "RT-GPON-37C8", "mab-mobile"}; //"GonioRad2.4G";//"DIR-615T-74CC"; // ssid = "ROSTELECOM_CB50";  "DIR-615T-74CC"    "M-Redmi"
const char *password[countWiFiAcc] = {"44220846", "Bgnagz3aL1", "Moward-WiFi"};   //"astz2023";//"44220846";    // password = "734GFTCN";      "44220846"  "Moward-WiFi"

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

void Browser(bool lampPosition[4], String formattedTime, String browserString1, String browserString2)
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
  client.println("<p>" + browserString1 + "</p>");

  client.println("<p>" + browserString2 + "</p>");

  client.println("<br>");
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

  // подключаемся к WiFi-сети:
  delay(500);
  for (int i = 0; i < countWiFiAcc; i++)
  {
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid[i]);
    WiFi.begin(ssid[i], password[i]);
    int j = 0;
    while (WiFi.status() != WL_CONNECTED & j < 20)
    {
      delay(500);
      Serial.print(".");
      j++;
      dali.transmit((adressParabFlux) << 1, 255);
    }
    if (WiFi.status() == WL_CONNECTED)
      break;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");

    // запускаем сервер:
    server.begin();
    Serial.println("Server started");
    delay(1);
    // печатаем IP-адрес:
    Serial.print("Use this URL to connect: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
  }
  else
  {
    Serial.print("Didnt connect to wifi");
  };

  switch (typeLightCntrl)
  {
  case 1:
    for (int i = 0; i < 4; i++)
    {
      pinMode(relayPin[i], OUTPUT);
    }
    break;

  case 2:
    pinMode(DALI_TX, OUTPUT);
    Serial.println("start DALI control");
    dali.setupTransmit(DALI_TX);        // устанавливаем цифровой выход для передачи сообщений
    dali.setupAnalogReceive(DALI_RX_A); // устанавливаем аналоговый вход для приема сообщений
    dali.msgMode = true;
    // dali.busTest(); // тестируем шину
    // dali.initialisation(); // Запустить инициализацию (присвоение коротких адресов).
    // dali.scanShortAdd();
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
  int currrentMin = (currentTime % 86400) / 60;

  int currrentMin48h = (currentTime % 172800) / 60;
  Serial.println(currrentMin48h);

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
        if ((currrentMin48h > onT) & (currrentMin48h < offT))
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
    dali.transmit((adressBlue) << 1, 0);
    delay(200);

    int ConstFlux = 0;
    byte ConstFluxCodeW = 0; // lum1 white

    if (currrentMin >= 7 * 60 & currrentMin < 21 * 60)
    {
      ConstFlux = maxConstFlux;
      ConstFluxCodeW = int(26.9 + 37.9 * log(ConstFlux));
    }
    else
      ConstFluxCodeW = 0;
    dali.transmit((adressConstFlux) << 1, ConstFluxCodeW);
    delay(200);
    Serial.println("Адрес: " + String(adressConstFlux) + ". Поток: " + String(ConstFlux));
    browserString1 = String(currrentMin) +
                     "<br> ConstFlux: adress = " + String(adressConstFlux) +
                     " flux = " + String(ConstFlux) +
                     " fluxCode = " + String(ConstFluxCodeW);

    // parabFlux = maxParabFlux * (-(1 / (3600*49)) * (currrentMin48h - 14 * 60) *(currrentMin48h - 14 * 60) + 1);
    int parabFlux;
    byte parabFluxCodeW = 0; // lum2 white
    float parabFluxCalc = (maxParabFlux * (1 - sq(float(currrentMin) - 14 * 60) / (3600 * 49)));
    if (parabFluxCalc > 0)
    {
      parabFlux = int(round(parabFluxCalc));
      parabFluxCodeW = int(25.1 + 37.9 * log(parabFlux));
    }
    else
    {
      parabFlux = 0;
      parabFluxCodeW = 0;
    }

    dali.transmit((adressParabFlux) << 1, parabFluxCodeW);
    Serial.println("Адрес: " + String(adressParabFlux) + ". Поток: " + String(parabFlux));
    browserString2 = String(currrentMin) +
                     " <br> parabFlux: Адрес= " + String(adressParabFlux) +
                     ". flux = " + String(parabFlux) +
                     ". fluxCode = " + String(parabFluxCodeW);
    break;
  }

  if (OTAEnabled)
  {
    HttpServer.handleClient(); // Прослушивание HTTP-запросов от клиентов
  }
  delay(5 * 1000);

  Browser(lampStatus, timeClient.getFormattedTime(), browserString1, browserString2);
}