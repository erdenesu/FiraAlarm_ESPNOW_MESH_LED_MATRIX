#include "Arduino.h"
#include "esp_wifi.h"
#include <esp_now.h>
#include <WiFi.h>
#include "Wifi_connection.h"
void setupMESH(bool Board_mode);
void loopMESH(uint16_t number_room,uint16_t self_num,uint16_t change_num,bool alarm,uint8_t blink_time,uint8_t blink_mode,bool is_set);
String using_device_data();
void write_RoomNumber(uint16_t nmr);
void write_blinkTime(uint8_t nmr);
void write_blinkMode(uint8_t nmr);
uint16_t read_RoomNumber();
uint8_t read_blinkTime();
uint8_t read_blinkMode();
uint16_t using_RoomNumber();
uint16_t client_change_number();
uint8_t client_change_time();
uint8_t client_change_mode();
bool _isNewData();
void _NewDataDone(bool stat);
void EEPROM_clearNUMBER();