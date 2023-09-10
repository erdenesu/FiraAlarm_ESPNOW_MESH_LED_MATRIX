#include "data_process.h"
uint16_t change_number[15] = {0};
uint16_t now_number[15] = {0};
uint8_t _mode[15] = {0};
uint8_t _time[15] = {0}; 
uint8_t _index = 0;
void get_String(String data){
    char c_data[data.length()];
    _index = 0;
    memset(change_number,0,sizeof(change_number));
    memset(now_number,0,sizeof(now_number));
    memset(_mode,0,sizeof(_mode));
    memset(_time,0,sizeof(_time));
    Serial.println("get data : " + data);
    strcpy(c_data,data.c_str());
    for(int i =0; i<data.length();i++){
        if(c_data[i]=='*' && c_data[i+1]=='*'){
            char numbers[15] = {0};
            for(int j=0;j<4;j++)if(c_data[i+j+2]!='*')numbers[j] = c_data[i+2+j];
            i++;
            uint16_t number  = (uint16_t)atoi(numbers);
            now_number[_index]=number;
            //Serial.println("-------NOW NUMBER----------- : "+String(number));
            
        }
        else if(c_data[i]=='*'){
            char numbers[15] = {0};
            for(int j=0;j<4;j++)if(c_data[i+j+1]!='M')numbers[j] = c_data[i+1+j];
            uint16_t number  = (uint16_t)atoi(numbers);
            change_number[_index]=number;
            //Serial.println("------CHANGE NUMBER----------- : "+String(number));
        }
        else if(c_data[i]=='M'){
            char numbers[15] = {0};
            for(int j=0;j<2;j++)if(c_data[i+j+1]!='T')numbers[j] = c_data[i+1+j];
            uint8_t number  = (uint8_t)atoi(numbers);
            _mode[_index]=number;
            //Serial.println("-------MODE  VALUE----------- : "+String(number));
        }
        else if(c_data[i]=='T'){
            char numbers[15] = {0};
            for(int j=0;j<2;j++)if(c_data[i+j+1]!='*')numbers[j] = c_data[i+1+j];
            uint8_t number  = (uint8_t)atoi(numbers);
            _time[_index]=number;
            //Serial.println("-------TIME  VALUE----------- : "+String(number));
            _index++;
        }
        else {};

    }
}
uint8_t get_index_count() {
    return _index;
}
uint8_t get_mode(uint8_t indx){
    return _mode[indx];
}
uint8_t get_time(uint8_t indx){
    return _time[indx];
}
uint16_t get_now_number(uint8_t indx){
    return now_number[indx];
}
uint16_t get_change_number(uint8_t indx){
    return change_number[indx];
}