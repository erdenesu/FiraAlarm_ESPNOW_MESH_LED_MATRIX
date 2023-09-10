#include "Wifi_connection.h"


const int EEPROM_SIZE=512;
bool wifi_connected=false;
bool WEB = false,fb_error=false;


const char* WIFIAP_SSID= "rk";
const char* WIFIAP_PASS ="dash4526";
const char* WIFI_SSID = "IW Charger";
const char* WIFI_PASS = "12345678";
const int wifi_w = 30;
const char* WIFI_NAME = "WIFI_NAME";// String type input
const char* WIFI_PASSWORD = "WIFI_PASSWORD";
const char* FIREBASE_LINK = "FIREBASE_LINK";// String type input
const char* FIREBASE_SEC = "FIREBASE_SECRET";
const char* BTN_WIFI = "BTN_WIFI";// String type input
const char* BTN_FIREBASE = "BTN_FIREBASE";
String WF_NAME,WF_PASS,FB_LINK,FB_SEC;

String Head = "<!DOCTYPE HTML><html><head>\
  <title>FIRE_ALARM</title> \
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \
  </head><body> \
  <form action=\"/get\"> \
  <h1>Fire Alarm </h1>";
String Wifi_design="<fieldset><legend> WiFi information </legend><center><p style=\"color :red;\">\  
    Wi-Fi name : <input type=\"text\" name=\"WIFI_NAME\"><br>\
    Wi-Fi pass : <input type=\"text\" name=\"WIFI_PASSWORD\"><br>\
    <input type=\"submit\" name=\"BTN_WIFI\" value=\"SAVE\" style\"background-color:green;\
    border-color:yellow;width:140px; height:80; font-size: 16px;border-radius: 12px;\
    margin-left:100px;\">\
   </p></center></fieldset>";
    //Firebase SECRET : <input type=\"text\" name=\"FIREBASE_SECRET\"><br>
String Fire_base= "<fieldset><legend> Firebase information </legend><center><p style=\"color :red;\">\  
    Firebase link : <input type=\"text\" name=\"FIREBASE_LINK\"><br>\
    Server secret code: <input type=\"text\" name=\"FIREBASE_SECRET\"><br>\
    <input type=\"submit\" name=\"BTN_FIREBASE\" value=\"SAVE\" style\"background-color:green;\
    border-color:yellow;width:140px; height:80; font-size: 16px;border-radius: 12px;\
    margin-left:100px;\">\
   </p></center></fieldset></form></body></html>";




WebServer server(8082);
String handle_AP=Head+Wifi_design+Fire_base; 
void handle_root_AP() {
  server.send(200, "text/html", handle_AP);
}
void toEEPROM_WIFI(String wfname,String wfpass){
  int saddr=2;
    EEPROM.writeString(saddr,wfname);
    saddr+= wfname.length()+1;
    EEPROM.writeString(saddr,wfpass);
    saddr+= wfpass.length()+1;
    EEPROM.commit();
}
void toEEPROM_FB(String fblink,String fb_secret){
  int saddr=50;
    EEPROM.writeString(saddr,fblink);
    saddr+= fblink.length()+1;
    EEPROM.writeString(saddr,fb_secret);
    saddr+= fb_secret.length()+1;
    EEPROM.commit();
    delay(500);
    ESP.restart();
}
void setupWebServer(){
     server.on("/setting", handle_root_AP);
     server.on("/get", HTTP_GET, []() {
      String inputMessage;
      String inputParam;
      //val++;
      if (server.hasArg(BTN_WIFI)) {
        WF_NAME=server.arg(WIFI_NAME);
        WF_PASS=server.arg(WIFI_PASSWORD);
        //Serial.println("WIFI NAME : "+ WF_NAME);
        //Serial.println("WIFI PASSWORD : "+ WF_PASS);
        toEEPROM_WIFI(WF_NAME,WF_PASS);
      }
      else if (server.hasArg(BTN_FIREBASE)) {
        FB_LINK=server.arg(FIREBASE_LINK);
        FB_SEC=server.arg(FIREBASE_SEC);
        Serial.println("DATABASE URL : "+ FB_LINK);
        Serial.println("DATABASE SECRET : "+ FB_SEC);
        //Serial.println(String(FB_SEC.length())+" : "+String(FB_LINK.length()));
        toEEPROM_FB(FB_LINK,FB_SEC);
        }
      else {
        inputMessage = "No Input Text sent on ESP32";
        inputParam = "none";
      }

      server.send(200, "text/html", Head+Wifi_design+Fire_base);
    });
   
  server.begin();
}

 void AP_MODE_SETUP(){
    String nameAP = "Room&Alarm";
    const int lenapN=nameAP.length();
    char APss[lenapN+1];
    //Serial.println("length  :  "+String(lenapN));
    nameAP.toCharArray(APss,lenapN+1);
    WIFIAP_SSID=APss;
    Serial.println("Name  :  "+String(WIFIAP_SSID));
    WiFi.softAP(WIFIAP_SSID, WIFIAP_PASS);
    //Serial.println();
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ") ;
    Serial.println(myIP);
    // Send web page with input fields to client
    setupWebServer();
    WEB=true;  
  }

  bool wifiConnecter(){
  int saddr=2;
  //String nami,pasi;
    String nami=EEPROM.readString(saddr);
    saddr+= nami.length()+1;
    String pasi=EEPROM.readString(saddr);
    saddr+= pasi.length()+1;
    
    
    const int lenN=nami.length(),lenP=pasi.length();
    char ss[lenN+1],pas[lenP+1];
    Serial.println("length  :  "+String(lenN)+" : "+String(lenP));
    nami.toCharArray(ss,lenN+1);
    pasi.toCharArray(pas,lenP+1);
    WIFI_SSID=ss;
    WIFI_PASS=pas;
    Serial.println("Wifi Name: "+String(ss));
    Serial.println("Wifi Password: "+String(pas));
    Serial.print("Wifi Name: ");
    Serial.println(WIFI_SSID);
    Serial.print("Wifi Password: ");
    Serial.println(WIFI_PASS);
    if(lenN>25)EEPROM_clear();
    if(lenN==0 || lenN>25)WiFi.begin("SK_WiFiGIGA476A","1805019736");
    else WiFi.begin(WIFI_SSID, WIFI_PASS);
    int n = 0;
    while(!wifi_connected){
       if (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        wifi_connected=false;
        }
      else{
        Serial.print("Connected : ");
        Serial.println(String(WIFI_SSID)+"\t");
        Serial.println(WiFi.localIP());
        wifi_connected=true;
        delay(500);
        break;
      }
      n++;
      if(n==wifi_w){
        AP_MODE_SETUP();
        Serial.println("Wifi status : "+String(WiFi.status()));
        delay(500);
        break;        
      }
    }return wifi_connected;
}


void WiFi_Setup(){
    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("Failed to initialise EEPROM");
        Serial.println("Restarting...");
        delay(1000);
        ESP.restart();
    }
    
    //WiFi.mode(WIFI_AP);
    //WiFi.softAP("DASH98733","12301234");
    handle_AP=Head+Wifi_design+Fire_base;
    AP_MODE_SETUP(); 
    wifiConnecter();

}
void handle_server(){
    server.handleClient();
}
void EEPROM_clear(){
  for(int i = 0;i<256; i++)EEPROM.write(i,0);
  EEPROM.commit();
  delay(2000);
  ESP.restart();
}