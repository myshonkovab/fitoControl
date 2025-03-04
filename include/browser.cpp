#include <ESP8266WiFi.h>             // OTA etc
#include <ESP8266WebServer.h>        // OTA
#include <ESP8266HTTPUpdateServer.h> //OTA
// #include <settings.cpp>
static bool daliScan;

WiFiServer server(80); //  порт веб-сервера

void Browser(bool relayStatus[4], String formattedTime, String browserString1, String browserString2)
{
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

  // CREATING WEB INTERFACE
  Serial.println(" Вывод информации браузера");

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

  client.println("<p> IP: ");
  client.print(WiFi.localIP());
  client.println("</p>");

  // Вывод текущих значений освещенности
  client.println("<p>Now " + formattedTime + "</p>");

  // ФОРМА ВВОДА
  client.println("<p> <form action=\"\">");
  client.println("<label for=\"GET-name\">Command:</label>");
  client.println("<input id=\"GET-name\" type=\"text\" name=\"command\">");
  client.println("<input type=\"submit\" value=\"Send\" ");
  client.println("</form> </p>");

  client.println("<p> Список команд: <br> scanDALI <br> dali1pin <br> rel3Addr<br> Lum1WAdr<br> Lum1RAdr<br> Lum2WAdr<br> Lum2RAdr<br>  r3Period<br> r3Duratn</p>");

  for (int i = 0; i < 3; i++)
  {
    client.println("relay" + String(i + 1) + " = " + String(!relayStatus[i]) + "<br>");
  }
  client.println("<p>" + browserString1 + "</p>");

  client.println("<p>" + browserString2 + "</p>");

  client.println("<br> <br> Click <a href=\":81/firmware\"> here to upload firmware </a> <br>");
  client.println("</body>");
  client.println("</html>");

  String request = client.readStringUntil('\r'); // считываем первую строчку запроса:
  client.flush();                                // сбрасывает все непрочитанные байты от сервера
  Serial.println(request);

  // обрабатываем запрос:
  if (request.indexOf("/?command=") != -1)
  {
    if (request.indexOf("scanDALI") != -1)
    {
      daliScan = true;
    }
    if (request.indexOf("dali1pin") != -1)
    {
      String s000 = request.substring(request.indexOf("dali1pin") + 8);
      if (s000.toInt() >= 0)
      {
        EEPROM[0] = s000.toInt();
        EEPROM.commit();
      }
    }
    if (request.indexOf("rel3Addr") != -1)
    {
      Serial.println("rel3Addr");
      String s000 = request.substring(request.indexOf("rel3Addr") + 8);
      Serial.println(s000);
      if (s000.toInt() >= 0)
      {
        EEPROM[3] = s000.toInt();
        EEPROM.commit();
      }
    }
    if (request.indexOf("Lum1WAdr") != -1)
    {
      String s000 = request.substring(request.indexOf("Lum1WAdr") + 8);
      if (s000.toInt() >= 0)
      {
        EEPROM[5] = s000.toInt();
        EEPROM.commit();
      }
    }
    if (request.indexOf("Lum1RAdr") != -1)
    {
      String s000 = request.substring(request.indexOf("Lum1RAdr") + 8);
      if (s000.toInt() >= 0)
      {
        EEPROM[7] = s000.toInt();
        EEPROM.commit();
      }
    }
    if (request.indexOf("Lum2WAdr") != -1)
    {
      String s000 = request.substring(request.indexOf("Lum2WAdr") + 8);
      if (s000.toInt() >= 0)
      {
        EEPROM[8] = s000.toInt();
        EEPROM.commit();
      }
    }
    if (request.indexOf("Lum2RAdr") != -1)
    {
      String s000 = request.substring(request.indexOf("Lum2RAdr") + 8);
      if (s000.toInt() >= 0)
      {
        EEPROM[10] = s000.toInt();
        EEPROM.commit();
      }
    }
    if (request.indexOf("r3Period") != -1)
    {
      Serial.println("r3Period");
      String s000 = request.substring(request.indexOf("r3Period") + 8);
      Serial.println(s000);
      if (s000.toInt() >= 0)
      {
        EEPROM[11] = s000.toInt();
        EEPROM.commit();
      }
    }
    if (request.indexOf("r3Duratn") != -1)
    {
      Serial.println("r3Duratn");
      String s000 = request.substring(request.indexOf("r3Duratn") + 8);
      Serial.println(s000);
      if (s000.toInt() >= 0)
      {
        EEPROM[12] = s000.toInt();
        EEPROM.commit();
      }
    }
  }
  client.stop();
  Serial.println("browser client disconnected");
}