#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "esp_now_test.h"
const char* ssid = "CPE_2.4G_113134";
const char* password = "12345678";

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int board_id;
  float temp;
  float hum;
  unsigned int Num_Readings;
} struct_message;

struct_message sensor_readings;


void data_receive(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  char macStr[18];
  Serial.print("Sensor Readings received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&sensor_readings, incomingData, sizeof(sensor_readings));
  
  Serial.printf("Board ID %u: %u bytes\n", sensor_readings.board_id, len);
  Serial.printf("Temperature value: %4.2f \n", sensor_readings.temp);
  Serial.printf("Humidity value: %4.2f \n", sensor_readings.hum);
  Serial.printf("Number of Readings: %d \n", sensor_readings.Num_Readings);
  Serial.println();
}

void setupESPNOW() {
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(data_receive);
}
 
void loopESPNOW() {
  static unsigned long last_time = millis();
  static const unsigned long interval = 5000;
  if ((millis() - last_time) > interval) {
   
    last_time = millis();
  }
}
#define board_ID 1

//MAC Address of the receiver 
uint8_t broadcastAddress[] = {0x7C, 0x9E, 0xBD, 0x37, 0x28, 0x4C};



unsigned long previous_time = 0;   
const long interval = 10000;    

unsigned int Num_Readings = 0;

constexpr char wifi_SSID[] = "Room12";

int32_t obtain_wifi(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}



void data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Readings Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setupTRANS() {

  int32_t channel = obtain_wifi(wifi_SSID);
  WiFi.printDiag(Serial); 
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  WiFi.printDiag(Serial);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }


  esp_now_register_send_cb(data_sent);
  
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.encrypt = false;       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loopTRANS() {
  unsigned long current_time = millis();
  if (current_time - previous_time >= interval) {
    previous_time = current_time;
    sensor_readings.board_id = board_ID;
    sensor_readings.temp = 15.21;
    sensor_readings.hum = 89;
    sensor_readings.Num_Readings = Num_Readings++;
     
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sensor_readings, sizeof(sensor_readings));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
  }
}