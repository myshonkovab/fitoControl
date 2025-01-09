Control of growbox lightning


1 - 1c
2 - 2б - 1
3 - 1к
4 - 2к
5 - 1б - 4
6 - 2с



old code and comments:

запись pinMode(0, OUTPUT); означает обращение к порту GPIO0
запись pinMode(D0, OUTPUT); означает обращение к порту D0 на плате


https://chip2.ru/esp8266/esp8266-nodemcu-v3-lua/ - распиновка

/* Turn on/off lamps for different periods
 */
// Объем памяти EEPROM = 4096 Б ? In Arduino you call EEPROM.begin(), but in ESP8266 you have to call EEPROM.begin(n), where n is the total number of bytes you will need.


/*Для первого эксперимента было:
{{19, 0, 47, 00}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}},
    {{9, 0, 23, 00}, {33, 0, 47, 0}, {-1, -1, -1, -1}, {-1, -1, -1, -1}},
    {{0, 0, 6, 0}, {11, 0, 18, 0}, {23, 0, 30, 0}, {35, 0, 42, 0}}, //6+7+7+7 - похоже ошибка была
    {{-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}},
*/

// uint8_t i2c_rcv = 255; // data received from I2C bus
// SoftwareSerial arduinoSerial(15, 13); //GPIO15 (TX) and GPIO13 (RX)

// контакт GPIO2 на ESP8266  - cвязан со светодиодом, который на плате
/// const byte lightPinMRGB[] = {15, 12, 13, 2}; //пин 0 - выход ШИМ первого светильника

