#include "espnow_mesh.h"
uint16_t ROOM_ADDRESS = 260;
uint16_t TIME_ADDRESS = 270;
uint16_t MODE_ADDRESS = 280;
bool is_new_data=false;
bool self_mode= false;
int BOARD_NUMBER = 7;
const int device_number = 7;
bool _isID=false;
int using_Devices=0;
int32_t channel=1;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  uint16_t id;
  uint16_t self_id;
  uint16_t change_id;
  bool alarm;
  uint8_t blink_mode;
  uint8_t blink_time;
  bool isSet;
} struct_message;

struct_message incomingReadings;
struct_message myData;
struct_message mySettingData;
struct_message sms[device_number]={incomingReadings};
unsigned int readingId = 0;

constexpr char WIFI_SSID[] = "Room&Alarm";

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  // Copies the sender mac address to a string
  char macStr[18];
  _isID = false;
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  if(incomingReadings.self_id==BOARD_NUMBER){
    mySettingData = incomingReadings;
    is_new_data = true;
  }
  for(int i=0;i<device_number;i++){
    if(sms[i].id==incomingReadings.id){
      sms[i]=incomingReadings;
      _isID=true;
    }
  }
  if(!_isID){
    sms[using_Devices]=incomingReadings;   
    if(using_Devices<device_number) using_Devices++;
  }
  Serial.printf("Board ID %d\n", incomingReadings.id);
  Serial.printf("self ID :  %d\n", incomingReadings.self_id);
  Serial.printf("Change ID :  %d\n", incomingReadings.change_id);
  Serial.printf("Alarm value: %d\n", incomingReadings.alarm);
  Serial.printf("time value: %d \n", incomingReadings.blink_time);
  Serial.printf("mode value: %d \n", incomingReadings.blink_mode);
  Serial.printf("Setted: %d \n", incomingReadings.isSet);
  Serial.println();
  
}


void setupMESH(bool Board_mode) {
  if (!EEPROM.begin(512)) {
        Serial.println("Failed to initialise EEPROM");
        Serial.println("Restarting...");
        delay(1000);
        ESP.restart();
    }
  self_mode = Board_mode;
  BOARD_NUMBER = read_RoomNumber();
  if(BOARD_NUMBER>5000){
    write_RoomNumber(0);
    BOARD_NUMBER = 0;
    delay(500);
  }
  Serial.println("----EEPROM BOARD NUMBER"+String(BOARD_NUMBER));
  if(BOARD_NUMBER<100)BOARD_NUMBER= random(100,5000);
  if(Board_mode){   
 
    WiFi.mode(WIFI_AP_STA);
    WiFi_Setup();
    Serial.print("Station IP Address: ");
    Serial.println(WiFi.localIP()); 
    channel = WiFi.channel();
  }
  else{ 
    WiFi.mode(WIFI_STA);  
    channel = getWiFiChannel(WIFI_SSID);

    WiFi.printDiag(Serial); // Uncomment to verify channel number before
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    WiFi.printDiag(Serial); // Uncomment to verify channel change after
  }
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  
}


void loopMESH(uint16_t number_room,uint16_t self_num,uint16_t change_num,bool alarm,uint8_t blink_time,uint8_t blink_mode,bool is_set) {
  if(self_mode)handle_server();
    //Serial.println("Room Number mesh : "+String(number_room));
    //Serial.println("Room self mesh : "+String(self_num));
    //Serial.println("Room change mesh : "+String(change_num));
    //Serial.println("Room alarm mesh : "+String(alarm));
    //Serial.println("Room time mesh : "+String(blink_time));
    //Serial.println("Room mode mesh : "+String(blink_mode));
    //Serial.println("Room state mesh : "+String(is_set));
    BOARD_NUMBER =number_room;
    //Serial.println("From EEEPROM ROOM NUMBER : "+String(EEPROM.readUInt(260)));
    //Serial.println("From EEEPROM ROOM NUMBER : "+String(read_RoomNumber()));
    //Serial.println("From EEEPROM ROOM mode : "+String(read_blinkMode()));
    //Serial.println("From EEEPROM ROOM time : "+String(read_blinkTime()));
    //peerInfo.encrypt = false;
    if (!esp_now_is_peer_exist(broadcastAddress))
    {
      Serial.println("Adding peer ....");
      esp_now_peer_info_t peerInfo;
      Serial.println("channel of WiFi : "+String(channel));
       //--------------------------------------------------
      
      if(!self_mode)
        ESP_ERROR_CHECK(esp_wifi_set_channel(channel ,WIFI_SECOND_CHAN_NONE));
      esp_now_del_peer(broadcastAddress);  
      memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));
      peerInfo.channel = channel;
      peerInfo.encrypt = false;
    
      memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    //-------------------------------------------------------
      if(esp_now_add_peer(&peerInfo)!=ESP_OK){
        Serial.println("Failed to add peer");
      };
      memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    }
    //---------------------------------------------------
    
    //------------------------------------------------
    //Set values to send
    myData.id = BOARD_NUMBER;
    myData.self_id = self_num;
    myData.change_id = change_num;
    myData.alarm = alarm;
    myData.blink_time = blink_time;
    myData.blink_mode = blink_mode;
    myData.isSet = is_set;
    //Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
}
String using_device_data(){
  String data_str = "";
  for(int o = 0; o<using_Devices;o++){
    data_str = data_str + "##"+String(sms[o].id);
    if(sms[o].alarm)data_str=data_str+"#T";
    else data_str=data_str+"#F";
    if(sms[o].isSet)data_str=data_str+"#T";
    else data_str=data_str+"#F";
  }
  data_str = data_str +"##"+String(myData.id);
  if(myData.alarm)data_str=data_str+"#T";
  else data_str=data_str+"#F";
  if(myData.isSet)data_str=data_str+"#T";
  else data_str=data_str+"#F";
  struct_message emp;
  for(int o = 0; o<using_Devices;o++){    
    sms[o]=emp;    
  }
  using_Devices =0;
  return data_str;
}
void write_RoomNumber(uint16_t nmr){
  if(nmr>99&&nmr<5000){
    EEPROM.writeUInt(ROOM_ADDRESS,nmr);
    EEPROM.commit();
  }
    
}
void write_blinkTime(uint8_t nmr){
  if(nmr>0&&nmr<10){
    EEPROM.writeUInt(TIME_ADDRESS,nmr);
    EEPROM.commit();
  }
    
}
void write_blinkMode(uint8_t nmr){
  if(nmr>0&&nmr<10){
    EEPROM.writeUInt(MODE_ADDRESS,nmr);
    EEPROM.commit();
  }
}
uint16_t read_RoomNumber(){
  return (uint16_t)EEPROM.readUInt(ROOM_ADDRESS);
}
uint8_t read_blinkTime(){
  return (uint8_t)EEPROM.readUInt(TIME_ADDRESS);
}
uint8_t read_blinkMode(){
  return (uint8_t)EEPROM.readUInt(MODE_ADDRESS);
}
uint16_t using_RoomNumber(){
  return BOARD_NUMBER;
}
uint16_t client_change_number(){
  return mySettingData.change_id;
}
uint8_t client_change_time(){
  return mySettingData.blink_time;
}
uint8_t client_change_mode(){
  return mySettingData.blink_mode;
}
bool _isNewData(){
  return is_new_data;
}
void _NewDataDone(bool stat){
  is_new_data = stat;
}
void EEPROM_clearNUMBER(){
  for(int w = 256; w>512;w++)EEPROM.write(w,0);
}