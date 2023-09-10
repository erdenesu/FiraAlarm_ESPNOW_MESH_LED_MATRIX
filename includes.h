#include "espnow_mesh.h"
#include "FB_connection.h"
#include "64x32Matrix.h"
#include "data_process.h"
#include "DF1201S.h"
#include <HardwareSerial.h>
#define SENSOR_PIN 48
#define MODE_PIN 47
//#define MAIN_MODE 1
//#define CLIENT_MODE 0
bool Board_Mode = false;
unsigned int long current_time_espmow =0;
int period_espnow = 2500;
unsigned int long current_time_FB =20000;
int period_FB = 10000;
void timed_espnow(int period);
void setup_MP3();