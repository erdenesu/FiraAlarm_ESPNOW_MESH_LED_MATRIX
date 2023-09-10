#include "Arduino.h"
#include "ETH.h"
#include "FirebaseESP32.h"
#include "EEPROM.h"
#include "WiFi.h"

void initFirebase();
void send_to_FB(String name, String data);
String get_from_FB(String name);