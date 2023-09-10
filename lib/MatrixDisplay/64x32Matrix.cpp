#include "64x32Matrix.h"

#define BAUD_RATE       115200  // serial debug port baud rate

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32         // Panel height of 64 will required PIN_E to be defined.


#define PANELS_NUMBER 2         // Number of chained panels, if just a single panel, obviously set to 1

MatrixPanel_I2S_DMA *matrix = nullptr;

void Mode_Matrix(uint8_t mode,uint16_t room_number){
  ///Right Arrow
  matrix->fillScreen(matrix->color444(0, 0, 0));
  for(int ind=0;ind<2;ind++){
    int shift = PANEL_WIDTH*ind; 
    int frame_w = PANEL_WIDTH+shift;
    if(ind==1){
        frame_w = 120;
        matrix->drawLine(127, 2, 127, 30, matrix->color444(5, 5, 0));
        matrix->drawLine(126, 2, 126, 30, matrix->color444(5, 5, 0));
    }
    matrix->drawRect(0+shift, 0,frame_w , PANEL_HEIGHT, matrix->color444(5, 5, 0));
    matrix->drawRect(1+shift, 1, frame_w-2, PANEL_HEIGHT-2, matrix->color444(5, 5, 0));
    matrix->setTextSize(2);
    matrix->setTextColor(matrix->color444(0,5,0));
    if(room_number>999)matrix->setCursor(10+shift, 9);
    else matrix->setCursor(15+shift, 9);
   matrix->print(String(room_number));
    //Serial.println("------SHIFT------ : " +String(shift));
  if(mode == 1){    
    matrix->drawLine(33+shift, 28, 58+shift, 28, matrix->color444(5, 5, 5));
    matrix->drawLine(33+shift, 27, 58+shift, 27, matrix->color444(5, 5, 5));
    matrix->drawLine(54+shift, 23, 57+shift, 26, matrix->color444(5, 5, 5));
    matrix->drawLine(53+shift, 23, 56+shift, 26, matrix->color444(5, 5, 5));
    matrix->drawLine(57+shift, 29, 54+shift, 32, matrix->color444(5, 5, 5));
    matrix->drawLine(56+shift, 29, 53+shift, 32, matrix->color444(5, 5, 5));
  }
   ///Left Arrow
  else if(mode == 2){
    matrix->drawLine(5+shift, 28, 30+shift, 28, matrix->color444(5, 5, 5));
    matrix->drawLine(5+shift, 27, 30+shift, 27, matrix->color444(5, 5, 5));
    matrix->drawLine(6+shift, 26, 9+shift, 23, matrix->color444(5, 5, 5));
    matrix->drawLine(7+shift, 26, 10+shift, 23, matrix->color444(5, 5, 5));
    matrix->drawLine(6+shift, 29, 9+shift, 32, matrix->color444(5, 5, 5));
    matrix->drawLine(7+shift, 29, 10+shift, 32, matrix->color444(5, 5, 5));
  }
  ///Up Arrow
  else if(mode == 3){
    ///Up Arrow right
    matrix->drawLine(57+shift, 5, 57+shift, 23, matrix->color444(5, 5, 5));
    matrix->drawLine(56+shift, 5, 56+shift, 23, matrix->color444(5, 5, 5));
    matrix->drawLine(58+shift, 6, 61+shift, 9, matrix->color444(5, 5, 5));
    matrix->drawLine(58+shift, 7, 61+shift, 10, matrix->color444(5, 5, 5));
    matrix->drawLine(52+shift, 9, 55+shift, 6, matrix->color444(5, 5, 5));
    matrix->drawLine(52+shift, 10, 55+shift, 7, matrix->color444(5, 5, 5));
    ///Up Arrow left
    matrix->drawLine(6+shift, 5, 6+shift, 23, matrix->color444(5, 5, 5));
    matrix->drawLine(7+shift, 5, 7+shift, 23, matrix->color444(5, 5, 5));
    matrix->drawLine(8+shift, 6, 11+shift, 9, matrix->color444(5, 5, 5));
    matrix->drawLine(8+shift, 7, 11+shift, 10, matrix->color444(5, 5, 5));
    matrix->drawLine(2+shift, 9, 5+shift, 6, matrix->color444(5, 5, 5));
    matrix->drawLine(2+shift, 10, 5+shift, 7, matrix->color444(5, 5, 5));
  }
  else {
    Serial.println("Alarm not detected");
  }
  }
}
void setupMatrix() {

  Serial.println("Starting pattern test...");

/*
#define R1_PIN_DEFAULT  7//25
#define G1_PIN_DEFAULT  39//26
#define B1_PIN_DEFAULT  6//27
#define R2_PIN_DEFAULT  5//14
#define G2_PIN_DEFAULT  40//12
#define B2_PIN_DEFAULT  4//13

#define A_PIN_DEFAULT   3//23
#define B_PIN_DEFAULT   41//19
#define C_PIN_DEFAULT   2//5
#define D_PIN_DEFAULT   42//17
#define E_PIN_DEFAULT   -1 // IMPORTANT: Change to a valid pin if using a 64x64px panel.
          
#define LAT_PIN_DEFAULT 46//4
#define OE_PIN_DEFAULT  38//15
#define CLK_PIN_DEFAULT 1//21 //16*/
  HUB75_I2S_CFG::i2s_pins _pins={
    7,//25, //R1_PIN, 
    39, //G1_PIN, 
    6, //B1_PIN, 
    5, //R2_PIN, 
    40, //G2_PIN, 
    4, //B2_PIN, 
    3, //A_PIN, 
    41, //B_PIN, 
    2, //C_PIN, 
    42, //D_PIN, 
    -1, //E_PIN,
    46, //LAT_PIN, 
    38, //OE_PIN, 
    1, //CLK_PIN
  };
  HUB75_I2S_CFG mxconfig(
    PANEL_WIDTH, // Module width
    PANEL_HEIGHT, // Module height
    PANELS_NUMBER, //, // chain length
    _pins // pin mapping -- uncomment if providing own custom pin mapping as per above.
  );
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::ICN2038S;

  // Display Setup
  matrix = new MatrixPanel_I2S_DMA(mxconfig);
  matrix->begin();
  matrix->setPanelBrightness(2);
}
uint8_t mode_alarm = 0;
uint16_t room_number = 485;
void testMatrix() {
  mode_alarm++;
  room_number+=6;
  Mode_Matrix(mode_alarm,room_number);
  if(mode_alarm>3)mode_alarm=0;
  if(room_number>990)room_number = 152;
}