Control of growbox lightning


old code and comments:

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

