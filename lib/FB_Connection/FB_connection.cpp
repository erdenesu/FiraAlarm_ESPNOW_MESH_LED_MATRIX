#include "FB_connection.h"

const char* DATABASE_URL ="https://test1-bc48f-default-rtdb.firebaseio.com/"; //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
const char* DATABASE_SECRET ="KWQtzuy14ZyCujKxAxXEYRKGiZTsKbcvfsh0MMwe"; 

bool fb_empty = false;
bool asas=false;

String get_data="";
FirebaseData fbdo;
int addr=50; 
void initFirebase(){
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
       
    String fblink=EEPROM.readString(addr);
    addr+= fblink.length()+1;
    String fbsec=EEPROM.readString(addr);
    addr+= fbsec.length()+1;
    const int lenfbl=fblink.length(),lenfbc=fbsec.length();
    char fblin[lenfbl],fbse[lenfbc+1];
    Serial.println("length  :  "+String(lenfbl)+" : "+String(lenfbc));
    fblink.toCharArray(fblin,lenfbl+1);
    fbsec.toCharArray(fbse,lenfbc+1);
    DATABASE_URL =fblin; 
    DATABASE_SECRET=fbse;
    if(lenfbl==0||lenfbc==0){
      fb_empty=true;
      Serial.println("Firebase information empty!!!!!");
      //return false; 
    }
    Serial.println("Data base URL: "+String(fblink));
    Serial.println("Data base Secret: "+String(fbsec));
    if(!fb_empty){
        Firebase.begin(DATABASE_URL,DATABASE_SECRET);//(DATABASE_URL, DATABASE_SECRET);
        Firebase.reconnectWiFi(true);
        Serial.println("Firebase Connecting...");
    //return true; 
    }
    else {
        DATABASE_URL ="https://test1-bc48f-default-rtdb.firebaseio.com/";
        DATABASE_SECRET ="KWQtzuy14ZyCujKxAxXEYRKGiZTsKbcvfsh0MMwe";
        Firebase.begin(DATABASE_URL,DATABASE_SECRET);//(DATABASE_URL, DATABASE_SECRET);
        Firebase.reconnectWiFi(true);
        Serial.println("Firebase Connecting...");
    }
  }

void send_to_FB(String name, String data){
  if (Firebase.ready()){
    asas=!asas;
    if(Firebase.RTDB.setString(&fbdo,"/"+name, data));
    //Serial.printf("Set data... %s\n", Firebase.setString(fbdo, F("/"+name), "Hello World!") ? "ok" : fbdo.errorReason().c_str());
    if(Firebase.RTDB.setString(&fbdo,"/WorkState", String(asas)));
   }
}
String get_from_FB(String name){
    char* ch_arr = new char[name.length()+1];
    strcpy(ch_arr,  ("/"+name).c_str());
    Serial.printf(ch_arr, get_data=Firebase.RTDB.getString(&fbdo,ch_arr) ? fbdo.to<const char *>() : fbdo.errorReason().c_str()); 
    //Serial.print(" : ");
    //Serial.println(get_data);
    return get_data;
}