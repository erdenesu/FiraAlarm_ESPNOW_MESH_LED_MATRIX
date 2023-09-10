#include "includes.h"
uint16_t nRoom;
bool _isSetting = false;
uint8_t _blinkTime =0;
uint8_t _blinkMode = 0;
#define WITHMESH

HardwareSerial DF1201SSerial (1);  //RX  TX

DFRobot_DF1201S DF1201S;

void setup(){
  Serial.begin(115200);
  
  pinMode(SENSOR_PIN,INPUT);
  pinMode(MODE_PIN,INPUT);
  setupMatrix();
  
  if(digitalRead(MODE_PIN)==HIGH){
    Board_Mode = true;
  }
  else {
    Board_Mode = false;
  }
  Serial.println(Board_Mode);
  setupMESH(Board_Mode);
  nRoom = using_RoomNumber();
  Serial.println("Room Number : "+String(nRoom));
  Mode_Matrix(0,nRoom);  
  setup_MP3();
  if(WiFi.status()==WL_CONNECTED)initFirebase();
  current_time_espmow = millis();
  current_time_FB = millis();
  _blinkTime = read_blinkTime();
  if(_blinkTime<1)_blinkTime=5;
  _blinkMode = read_blinkMode();
  
  
}
void loop(){
 timed_espnow(period_espnow);
}
int u =0;
void timed_espnow(int period){
  //String _data_send = using_device_data();
  if(millis()-current_time_espmow>period){     
   if(WiFi.status()==WL_CONNECTED && Board_Mode){
    get_String(get_from_FB("SETTING")); 
      uint16_t _nNum = get_now_number(u);
       if(nRoom==_nNum)  {
        uint8_t _tTime = get_time(u);
        uint8_t _mMode = get_mode(u);
        uint16_t _rRoom = get_change_number(u);
        if(_blinkTime!=_tTime &&_tTime!=0){
          _blinkTime = _tTime;
          write_blinkTime(_tTime);
        }
        if(_blinkMode!=_mMode && _mMode !=0){
          _blinkMode = _mMode;
          write_blinkMode(_mMode);
        }
        if(nRoom!=_rRoom && _rRoom > 99){
          write_RoomNumber(_rRoom);
          nRoom = _rRoom;
          Mode_Matrix(0,nRoom);
        } 
        if((_blinkTime!=_tTime &&_tTime!=0)||(_blinkMode!=_mMode && _mMode !=0)||(nRoom!=_rRoom  && _rRoom > 99)){
          _isSetting = !_isSetting;          
        }
       }
       else {
          loopMESH(nRoom,_nNum,get_change_number(u),digitalRead(SENSOR_PIN),get_time(u),get_mode(u),_isSetting);
       }
       u++;
       if (u==get_index_count()&&u!=0){
          u=0;
          send_to_FB("MESH",using_device_data());
        }
       
      //}
    //Serial.println("-----------INDEX NUMBER -----------"+String(get_index_count()));
   }else{
      if(_isNewData()){
        uint8_t _tTime = client_change_time();
        uint8_t _mMode = client_change_mode();
        uint16_t _rRoom = client_change_number();
        if((_blinkTime!=_tTime &&_tTime!=0)||(_blinkMode!=_mMode && _mMode !=0)||(nRoom!=_rRoom  && _rRoom > 99)){
          if(_blinkTime!=_tTime && _tTime !=0 && _tTime <10){
            _blinkTime = _tTime;
            write_blinkTime(_tTime);
          }
          if(_blinkMode!=_mMode && _mMode !=0 && _mMode < 10){
            _blinkMode = _mMode;
            write_blinkMode(_mMode);
          }
          if(nRoom!=_rRoom && _rRoom > 99 && _rRoom < 5000){
            write_RoomNumber(_rRoom);
            nRoom = _rRoom;
            Mode_Matrix(0,nRoom);
          }
          _isSetting = !_isSetting;
          Serial.println("Sending LoRa data...");
          loopMESH(nRoom,0,0,digitalRead(SENSOR_PIN),1,5,_isSetting);
        }
        _NewDataDone(false);
      }else loopMESH(nRoom,0,0,digitalRead(SENSOR_PIN),1,5,_isSetting);
   }
   //testMatrix();
   current_time_espmow = millis();
  }
 
}
void setup_MP3(){
    DF1201SSerial.begin(115200,SERIAL_8N1,36,35,false);
  while(!DF1201S.begin(DF1201SSerial)){
    Serial.println("Init failed, please check the wire connection!");
    delay(1000);
  }
  DF1201S.setVol(/*VOL = */15);
  Serial.print("VOL:");
  /*Get volume*/
  Serial.println(DF1201S.getVol());
  /*Enter music mode*/
  DF1201S.switchFunction(DF1201S.MUSIC);
  DF1201S.setPlayMode(DF1201S.SINGLE);
  Serial.print("PlayMode:");
  Serial.println(DF1201S.getPlayMode());
 
  Serial.print("All Name:");
  Serial.println(DF1201S.getTotalFile());
  
  DF1201S.start();
  DF1201S.playSpecFile("/"+String(nRoom)+".mp3"); 
  Serial.print("File Name:");
  Serial.println(DF1201S.getFileName());
  //DF1201S.playFileNum(2);
}