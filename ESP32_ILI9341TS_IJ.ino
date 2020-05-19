/***************************************************
  ESP32 ILI9341 Touch Screen for IchigoJam
  CC by Michio Ono
  
 ****************************************************/

//#include <Adafruit_GFX.h>
//#include <Adafruit_ILI9341.h>

//this is needed even tho we aren't using it
//#include <Wire.h>
//#include <SPI.h>
//HSPI - SCK,MISO,MOSI,SS = 14,12,13,15
//VSPI - SCK,MISO,MOSI,SS = 18,19,23,5    <--- ESP32 default

#include <XPT2046_Touchscreen.h>////#include <Adafruit_STMPE610.h>

//This is calibration data 
//for the raw touch data to the screen coordinates

//Width
#define TS_MINX 370
#define TS_MAXX 3800
#define tftWidth  320

//Height
#define TS_MINY 370
#define TS_MAXY 3800
#define tftHeight 240

//Pressure
#define TS_MINZ 600
#define TS_MAXZ 2400
#define tftPress  100

//Library of touch screen
//#define T_CLK   18  //SCK   default
  #define T_CS    15  //TFT_CS ... GND
//#define T_DIN   23  //MOSI  default
//#define T_DO    19  //MISO  default
//#define T_IQR   0   //In the case of IRQ is NULL or 255, work without polling

XPT2046_Touchscreen ts=XPT2046_Touchscreen(T_CS);
//XPT2046_Touchscreen ts=XPT2046_Touchscreen(T_CS,T_IQR);

void setup(void) {
  
  //Initialize serial
  Serial.begin(115200);
  Serial.println(F("Touch Paint!"));
  Serial2.begin(115200, SERIAL_8N1, 16, 17);//Rx=16,Tx=17

  //Initialize touch screen
  if(!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while(1);//stop
  }

  //rotation screen
  ts.setRotation(1);
  Serial.println("Touchscreen started");
  Serial2.flush();
}

TS_Point p;
TS_Point pp;//previous data//

boolean wastouched = false;
boolean istouched;
char    tsp[20];

String inStr = ""; // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

enum {
  kTS_none=0,
  kTS_XYZ,
  kTS_XY,
  kTS_XZ,
  kTS_YZ,
  kTS_X,
  kTS_Y,
  kTS_Z,
  kTS_HV,
  kTS_HZ,
  kTS_VZ,
  kTS_H,
  kTS_V,
};
int tsr=kTS_none;

enum {
  kIJ_LEFT=28,
  kIJ_RIGHT,
  kIJ_UP,
  kIJ_DOWN
};

int hBdr=tftWidth/2;
int vBdr=tftHeight/2;
#define ts_mov_diff 3
#define ts_send_interval 5
//#define ts_H_Edge 160
//#define ts_V_Edge 120

void loop()
{

  while(Serial2.available()) {
    char inChar = (char)Serial2.read();
    //Check end of line 
    if(inChar=='\n' || inChar=='\r') {
      stringComplete=true;
      Serial2.flush();
      Serial.println(inStr);
      break;
    } else {
      inStr += inChar;
    }
    //delay(1);
  }

  if(stringComplete) {
    stringComplete=false;
    String cs=inStr;//文字をコピー
    cs.toUpperCase();//大文字に
    if(cs.startsWith("TS HV")) { //水平方向
      tsr=kTS_HV;
    } else if(cs.startsWith("TS H")) { //垂直方向
      tsr=kTS_H;
    } else if(cs.startsWith("TS V")) { //垂直方向
      tsr=kTS_V;
    } else if(cs.startsWith("TS XYZ")) { //座標・筆圧
      tsr=kTS_XYZ;
    } else if(cs.startsWith("TS XY")) { //座標
      tsr=kTS_XY;
    } else if(cs.startsWith("TS XZ")) { //X,圧
      tsr=kTS_XZ;
    } else if(cs.startsWith("TS YZ")) { //Y,圧
      tsr=kTS_YZ;
    } else if(cs.startsWith("TS X")) { //X
      tsr=kTS_X;
    } else if(cs.startsWith("TS Y")) { //Y
      tsr=kTS_Y;
    } else if(cs.startsWith("TS Z")) { //Z
      tsr=kTS_Z;
    } else {
      tsr=kTS_none;
    }
  }
  
  // See if there's any  touch data for us
  //if(!ts.bufferEmpty()) {
  //istouched = ts.touched();
  if(ts.touched()) {
    // Retrieve a point  
    p=ts.getPoint();
    if(wastouched==false) {
      wastouched=true;
      pp=p;
    }
    
    p.x = min(max((int32_t)map(p.x, TS_MINX, TS_MAXX, 0, tftWidth ),0),tftWidth );
    p.y = min(max((int32_t)map(p.y, TS_MINY, TS_MAXY, 0, tftHeight),0),tftHeight);
    p.z = min(max((int32_t)map(p.z, TS_MINZ, TS_MAXZ, 0, tftPress),0),tftPress);
    
    if(tsr) { //stringComplete      
      if(tsr>kTS_HV) { //HV方向
        if(tsr==kTS_H) { //水平方向
          if(p.x<pp.x-ts_mov_diff) {
            Serial2.write(kIJ_LEFT);
            hBdr=min(p.x+ts_mov_diff,tftWidth-ts_mov_diff-1);
          } else if(p.x>pp.x+ts_mov_diff) {
            Serial2.write(kIJ_RIGHT);
            hBdr=max(p.x-ts_mov_diff,ts_mov_diff+1);
          } else if(p.x<hBdr) {
            Serial2.write(kIJ_LEFT);
            hBdr=min(p.x+ts_mov_diff,tftWidth-ts_mov_diff-1);
          } else if(p.x>hBdr) {
            Serial2.write(kIJ_RIGHT);
            hBdr=max(p.x-ts_mov_diff,ts_mov_diff+1);
          } /*else if(p.x<ts_H_Edge) { //左の方をタッチしてた時
            Serial2.write(kIJ_LEFT);
          } else if(p.x>tftWidth-ts_H_Edge) { //右の方をタッチしてた時
            Serial2.write(kIJ_RIGHT);
          } else {
            Serial2.write(0);
          }*/
          //delay(ts_send_interval);
          
        } else if(tsr==kTS_V) { ///垂直方向
          if(p.y<pp.y-ts_mov_diff) {
            Serial2.write(kIJ_UP);
            vBdr=min(p.y+ts_mov_diff,tftHeight-ts_mov_diff-1);
          } else if(p.y>pp.y+ts_mov_diff) {
            Serial2.write(kIJ_DOWN);
            vBdr=max(p.y-ts_mov_diff,ts_mov_diff+1);
          } else if(p.y<vBdr) {
            Serial2.write(kIJ_UP);
            vBdr=min(p.y+ts_mov_diff,tftHeight-ts_mov_diff-1);
          } else if(p.y>vBdr) {
            Serial2.write(kIJ_DOWN);
            vBdr=max(p.y-ts_mov_diff,ts_mov_diff+1);
          } /*else if(p.y<ts_V_Edge) { //上の方をタッチしてた時
            Serial2.write(kIJ_UP);
          }  else if(p.y>tftHeight-ts_V_Edge) { //下の方をタッチしてた時
            Serial2.write(kIJ_DOWN);
          } else {
            Serial2.write(0);
          }*/
          //delay(ts_send_interval);
        }
  
      } else { //座標・筆圧
        
        p.x=min(max((p.x-32)/8,0),31);//p.x=min(max(p.x-32,0),255)/8;
        p.y=min(max((p.y-24)/8,0),23);//p.x=min(max(p.x-24,0),191)/8;
        p.z=min(max((p.z+24)/25,1),4);
        
        if(tsr==kTS_XYZ) {
          uint8_t d[]={p.x,p.y,p.z}; Serial2.write((uint8_t*)d,3); Serial2.flush();
        } else if(tsr==kTS_XY) {
          uint8_t d[]={p.x,p.y}; Serial2.write((uint8_t*)d,2); Serial2.flush();
        } else if(tsr==kTS_XZ) {
          uint8_t d[]={p.x,p.z}; Serial2.write((uint8_t*)d,2); Serial2.flush();
        } else if(tsr==kTS_YZ) {
          uint8_t d[]={p.y,p.z}; Serial2.write((uint8_t*)d,2); Serial2.flush();
        } else if(tsr==kTS_X) {
          Serial2.write((uint8_t)p.x); Serial2.flush();
        } else if(tsr==kTS_Y) {
          Serial2.write((uint8_t)p.y); Serial2.flush();
        } else if(tsr==kTS_Z) {
          Serial2.write((uint8_t)p.z); Serial2.flush();
        }
      }
    }
    
    int n=sprintf(tsp,"'%03d,%03d,%03d\n",p.x,p.y,p.z);//tsp[n]=0;
    Serial.print(tsp);
    pp.x=p.x; pp.y=p.y; pp.z=p.z;

  } else { //タッチしてない
    wastouched=false;
    while(!ts.bufferEmpty()) { p = ts.getPoint();}
    p.x=p.y=p.z=0;
  }
  
  tsr=kTS_none;
  inStr="";
  delay(1);
  
}
