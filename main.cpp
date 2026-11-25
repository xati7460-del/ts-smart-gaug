/*
  TS SMART GAUGE - RACING EDITION
  Board: ESP32-2432S028 CYD, ESP32-WROOM-32, ST7789 label 7789
  Panel: 240x320 rotated landscape => 320x240
  Library: TFT_eSPI with included TFT_eSPI_User_Setup.h
  Demo serial format at 115200: S85,R6500,G3,F72,T88,V13.8
  NOTE: demo telemetry only; real motorcycle sensors are not connected.
*/
#include <TFT_eSPI.h>
TFT_eSPI tft;

#define TFT_BL 21
#define C_BLACK  0x0000
#define C_WHITE  0xFFFF
#define C_CYAN   0x07FF
#define C_BLUE   0x001F
#define C_GREEN  0x07E0
#define C_YELLOW 0xFFE0
#define C_ORANGE 0xFD20
#define C_RED    0xF800
#define C_GREY   0x9CF3
#define C_DARK   0x0862
#define C_LINE   0x31A6

int speedKmh=0, rpm=0, gear=0, fuelPct=0, tempC=0, maxSpeed=0;
float voltage=0.0f;
String rxLine;

void drawHeader() {
  tft.fillRect(0,0,320,38,C_BLACK);
  tft.fillTriangle(0,0,120,0,0,38,0x4000);
  tft.setTextColor(C_YELLOW,C_BLACK); tft.drawString("TS",12,8,4);
  tft.setTextColor(C_WHITE,C_BLACK); tft.drawString("SMART GAUGE",54,14,2);
  tft.setTextColor(C_ORANGE,C_BLACK); tft.drawRightString("RACING",306,8,2);
  tft.setTextColor(C_GREY,C_BLACK); tft.drawRightString("BY CHANG SUEA",306,25,1);
  tft.drawFastHLine(8,37,304,C_ORANGE);
  tft.drawFastHLine(8,39,304,C_CYAN);
}

void drawRpmScale() {
  tft.setTextColor(C_GREY,C_BLACK); tft.drawString("RPM",12,45,2);
  tft.drawRoundRect(53,44,254,17,4,C_LINE);
  for(int i=0;i<20;i++){
    uint16_t c = i<10 ? C_GREEN : (i<14 ? C_YELLOW : (i<17 ? C_ORANGE : C_RED));
    tft.fillRoundRect(57+i*12,48,8,9,2,c);
  }
  tft.fillRect(57,48,240,9,C_BLACK);
  tft.drawString("0",14,63,1);
  tft.drawRightString("12K",306,63,1);
}

void drawStatic() {
  tft.fillScreen(C_BLACK);
  // diagonal racing stripes and grid
  for(int x=-120;x<360;x+=32) tft.drawLine(x,0,x+100,240,0x1082);
  tft.fillTriangle(0,75,8,75,0,155,C_RED);
  tft.fillTriangle(312,75,320,75,320,155,C_CYAN);
  drawHeader();
  drawRpmScale();

  // Main speed panel
  tft.fillRoundRect(10,82,184,108,10,C_DARK);
  tft.drawRoundRect(10,82,184,108,10,C_CYAN);
  tft.drawRoundRect(13,85,178,102,8,C_ORANGE);
  tft.setTextColor(C_CYAN,C_DARK); tft.drawString("SPEED",22,91,2);
  tft.setTextColor(C_WHITE,C_DARK); tft.drawString("000",22,112,7);
  tft.setTextColor(C_YELLOW,C_DARK); tft.drawString("km/h",140,157,2);
  tft.drawFastHLine(22,177,160,C_ORANGE);
  tft.setTextColor(C_GREY,C_DARK); tft.drawString("MAX",24,179,1);

  // Gear tile
  tft.fillRoundRect(204,82,106,66,8,0x2000);
  tft.drawRoundRect(204,82,106,66,8,C_ORANGE);
  tft.setTextColor(C_YELLOW,0x2000); tft.drawString("GEAR",215,89,2);
  tft.drawRoundRect(246,107,43,35,5,C_CYAN);

  // Engine status tile
  tft.fillRoundRect(204,155,106,35,7,0x0841);
  tft.drawRoundRect(204,155,106,35,7,C_RED);
  tft.setTextColor(C_GREY,0x0841); tft.drawString("ENGINE",212,158,1);
  tft.fillCircle(286,166,5,C_GREEN);
  tft.setTextColor(C_GREEN,0x0841); tft.drawString("READY",212,175,1);

  // Bottom telemetry cards
  const int x[3]={10,115,220};
  const char* label[3]={"FUEL","TEMP","VOLT"};
  for(int i=0;i<3;i++){
    uint16_t edge = i==0?C_GREEN:(i==1?C_ORANGE:C_CYAN);
    tft.fillRoundRect(x[i],199,90,29,5,C_DARK);
    tft.drawRoundRect(x[i],199,90,29,5,edge);
    tft.setTextColor(edge,C_DARK); tft.drawString(label[i],x[i]+7,201,1);
  }
}

void drawLive() {
  // Speed
  tft.fillRect(20,112,158,54,C_DARK);
  char buf[8]; snprintf(buf,sizeof(buf),"%03d",speedKmh);
  tft.setTextColor(C_WHITE,C_DARK); tft.drawString(buf,22,112,7);

  // RPM live bar overlay
  tft.fillRect(56,47,246,12,C_BLACK);
  int n=constrain(rpm/600,0,20);
  for(int i=0;i<n;i++){
    uint16_t c=i<10?C_GREEN:(i<14?C_YELLOW:(i<17?C_ORANGE:C_RED));
    tft.fillRoundRect(57+i*12,48,8,9,2,c);
  }
  tft.fillRect(212,64,94,12,C_BLACK);
  tft.setTextColor(C_CYAN,C_BLACK);
  tft.drawRightString(String(rpm)+" RPM",306,64,1);

  // Gear
  tft.fillRect(249,110,37,29,0x2000);
  tft.setTextColor(C_YELLOW,0x2000);
  tft.drawCentreString(gear==0?"N":String(gear),267,108,4);

  // Bottom telemetry
  tft.fillRect(17,214,76,11,C_DARK);
  tft.fillRect(122,214,76,11,C_DARK);
  tft.fillRect(227,214,76,11,C_DARK);
  tft.setTextColor(C_WHITE,C_DARK);
  tft.drawString(String(fuelPct)+"%",18,214,1);
  tft.drawString(String(tempC)+" C",123,214,1);
  tft.drawString(String(voltage,1)+" V",228,214,1);

  if(speedKmh>maxSpeed) maxSpeed=speedKmh;
  tft.fillRect(63,179,112,9,C_DARK);
  tft.setTextColor(C_GREY,C_DARK);
  tft.drawString(String(maxSpeed)+" km/h",64,179,1);
}

void parseTelemetry(String s) {
  s.trim(); int p=0;
  while(p<s.length()){
    int q=s.indexOf(',',p);
    String tok=(q<0)?s.substring(p):s.substring(p,q);
    if(tok.length()>1){
      char k=tok.charAt(0); String v=tok.substring(1); v.trim();
      if(k=='S') speedKmh=max(0,v.toInt());
      if(k=='R') rpm=max(0,v.toInt());
      if(k=='G') gear=constrain(v.toInt(),0,6);
      if(k=='F') fuelPct=constrain(v.toInt(),0,100);
      if(k=='T') tempC=v.toInt();
      if(k=='V') voltage=v.toFloat();
    }
    if(q<0) break;
    p=q+1;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TFT_BL,OUTPUT); digitalWrite(TFT_BL,HIGH);
  tft.init();
  tft.setRotation(1);
  drawStatic();
  drawLive();
  Serial.println("TS SMART GAUGE RACING ready");
  Serial.println("Send: S85,R6500,G3,F72,T88,V13.8");
}

void loop() {
  while(Serial.available()){
    char c=(char)Serial.read();
    if(c=='\n'){ parseTelemetry(rxLine); rxLine=""; drawLive(); }
    else if(c!='\r' && rxLine.length()<100) rxLine+=c;
  }
}
