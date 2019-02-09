/* Yasin EKEN
 * 14.03.2017
 * Arduino 5110 LCD Oscilloscope
 * http://arduproje.blogspot.com.tr
 * https://www.facebook.com/groups/Arduino.Pic/?ref=bookmarks
 * https://www.facebook.com/ArduinoPicGelistirmePlatformu
 * https://www.youtube.com/user/runcode1/videos
 */
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <FreqCount.h> 
#include <PWM.h>
#include <EEPROM.h>
#include "Waveforms.h"
#define Ekran     8
#define led       9   
#define dds       10  
#define key_down  13  
#define key_ok    12  
#define key_up    11  
#define akb       A2   
#define P         A5   
#define overclock 16  
#define cof 0
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 4, 3, 2);       
byte SinU=30;  
int PWM = 50;         
int32_t freq = 500; 
float VCC=5.0;  
byte d=0;
byte hag=0;
int stepFreq=0;
bool flag=0;
byte mass[701];
byte x=0; 
byte ddsMenu=0; 
byte oscMenu=0; 
bool opornoe=1; 
bool pause=0; 
byte mainMenu=0;  
byte razv=6;  
unsigned long count=0;
byte sinX=30; 
byte meaX=83;
int Vmax=0;
byte sinhMASS=0;
long countX=0;
long speedTTL=9600; 
byte STTL=4;
int  prokr=0;
bool flag_key;
byte contrast;
byte Set=0;
bool BL;
byte hs_freq=0;
byte hs=0;
byte x_line=47;
//************************************************************************************************************
void Generator() {
  display.clearDisplay();  
  if (flag==0) {
    if(flag_key-!digitalRead(key_down)){
      if(freq<100000) { freq-=stepFreq; if(freq<1) freq=1; bool success = SetPinFrequencySafe(led, freq); pwmWrite(led, PWM*2.55);} else
      {
        hs_freq--;
        if (hs_freq == 0 ) { freq-=stepFreq;}
        if (hs_freq == 1 ) { freq=100000; hs  = 79;}
        if (hs_freq == 2 ) { freq=250000; hs  = 31;}
        if (hs_freq == 3 ) { freq=500000; hs  = 15;}
        if (hs_freq == 4 ) { freq=1000000;hs = 7;}
        if (hs_freq == 5 ) { freq=2000000;hs = 3;}
        if (hs_freq == 6 ) { freq=4000000;hs = 1;}
        if (hs_freq == 7 ) { freq=8000000;hs = 0;}
        cli();
        TCCR1A = bit (COM1A0);               
        TCCR1B = bit (WGM12) | bit (CS10);  
        OCR1A = hs;
        sei();
      }
      delay(50); 
    }
    if(flag_key-!digitalRead(key_up)){
      if(freq<100000) { freq+=stepFreq; bool success = SetPinFrequencySafe(led, freq); pwmWrite(led, PWM*2.55);} else
      {
        hs_freq++;
        if (hs_freq == 1 ) { freq=100000; hs = 79;}
        if (hs_freq == 2 ) { freq=250000; hs = 31;}
        if (hs_freq == 3 ) { freq=500000; hs = 15;}
        if (hs_freq == 4 ) { freq=1000000;hs = 7;}
        if (hs_freq == 5 ) { freq=2000000;hs = 3;}
        if (hs_freq == 6 ) { freq=4000000;hs = 1;}
        if (hs_freq == 7 ) { freq=8000000;hs = 0;}
        if (hs_freq == 8 ) hs_freq=7;
        cli();
        TCCR1A = bit (COM1A0);        
        TCCR1B = bit (WGM12) | bit (CS10);  
        OCR1A = hs;
        sei();
      }
      delay(50); 
    }  
  }
  if (flag==1) {
    InitTimersSafe();
    bool success = SetPinFrequencySafe(led, freq); 
    pwmWrite(led, PWM*2.55);
    if(flag_key-!digitalRead(key_down)){
      PWM=PWM-1;
      if(PWM<0) PWM=0; 
      delay(50);
    }
    if(flag_key-!digitalRead(key_up)){
      PWM=PWM+1;
      if(PWM>100) PWM=100; 
      delay(50); 
    }  
  }      
  if(flag_key-!digitalRead(key_ok)) {   
    delay(100);
    hag++;
    if(hag>=6) hag=0;
  } 
  display.setTextSize(1);
  display.setCursor(21,5);
  display.print("PWM=");
  display.print(PWM);
  display.print("%");
  display.drawLine(41-(41*PWM/100.0),  0, 42+(41*PWM/100.0),  0, BLACK);
  display.drawLine(42-(41*PWM/100.0),  1, 41+(41*PWM/100.0),  1, BLACK);
  display.drawLine(43-(41*PWM/100.0),  2, 40+(41*PWM/100.0),  2, BLACK);
  display.drawLine(41-(41*PWM/100.0), 16, 42+(41*PWM/100.0), 16, BLACK);
  display.drawLine(42-(41*PWM/100.0), 15, 41+(41*PWM/100.0), 15, BLACK);
  display.drawLine(43-(41*PWM/100.0), 14, 40+(41*PWM/100.0), 14, BLACK);   
  display.setTextSize(2);
  long freqX=freq*(overclock/16.0);
  if(freqX<1000){
    display.setCursor(25, 20); display.print(freqX); 
    display.setTextSize(1);    display.print("Hz"); 
  } else if(freqX<10000){
    display.setCursor(18, 20); display.print(freqX); 
    display.setTextSize(1);    display.print("Hz"); 
  }else if(freqX<100000){
    display.setCursor(11, 20); display.print(freqX); 
    display.setTextSize(1);    display.print("Hz"); 
  } else {
  if(freqX<1000000)   {display.setCursor(5, 20);display.print(freqX/1000.0, 1); display.setTextSize(1);    display.print("KHz");} else
  if(freqX<10000000)  {display.setCursor(24, 20);display.print(freqX/1000000.0, 1); display.setTextSize(1); display.print("MHz");} 
  }
  display.setCursor(0,40);
  display.setTextSize(1);
  display.print(">>");
  display.setTextColor(WHITE, BLACK);
  display.print(" ");
  if(freq<100000){
    if(hag<5) display.print("x");    
    if(hag==0) {       
      display.print(1*(overclock/16.0),0); 
      stepFreq=1;
      flag=0;
    }
    if(hag==1){
      display.print(10*(overclock/16.0),0); 
      stepFreq=10;
    }
    if(hag==2){
      display.print(100*(overclock/16.0),0); 
      stepFreq=100;
    }
    if(hag==3){
      display.print(1000*(overclock/16.0),0); 
      stepFreq=1000;
    }
    if(hag==4){
      display.print(10000*(overclock/16.0),0); 
      stepFreq=10000;
    } 
  } else display.print("HS FREQ");
  if(hag==5){
    display.setCursor(0,40);
    display.setTextSize(1);
    display.print(">>");
    display.setTextColor(WHITE, BLACK);
    display.print(" ");
    display.print("PWM ");
    display.print(PWM);
    display.print("%"); 
    flag=1;
  }
  display.print(" ");
  display.setTextColor(BLACK);
  display.print("<<");
  delay(70);
  display.display();
}
void DDSGeneratorMenu() {
  display.clearDisplay(); 
  if (ddsMenu==0) display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
  display.setCursor(0, 0); display.println("     Sine     ");
  if (ddsMenu==1) display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
  display.setCursor(0,10); display.println("  Triangular  ");
  if (ddsMenu==2) display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
  display.setCursor(0,20); display.println("   Sawtooth   ");
  if (ddsMenu==3) display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
  display.setCursor(0,30); display.println("    Square    ");
  if (ddsMenu==4) display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
  display.setCursor(0,40); display.println("     ECG      ");
  display.setTextColor(BLACK);
  display.display();
}
void DDSGenerator(){
  if(ddsMenu==0) {
    DDSGeneratorMenu();    
    while(flag_key-digitalRead(key_up) && flag_key-digitalRead(key_down) && flag_key-digitalRead(key_ok)) {
      //PWM=pgm_read_byte(&(sine[d]));
      PWM=pgm_read_byte(&(waveformsTable[0][d]));
      pwmWrite(dds,PWM);
      d++;
      if(d==255) d=0;
    }
    ddsMenu++;
    delay(200);
  }
  if(ddsMenu==1) {
    DDSGeneratorMenu();    
    while(flag_key-digitalRead(key_up) && flag_key-digitalRead(key_down) && flag_key-digitalRead(key_ok)) {
      //PWM=trianglM[d];
      PWM=pgm_read_byte(&(waveformsTable[1][d]));
      pwmWrite(dds,PWM);
      d++;
      if(d==255) d=0;
    }
    ddsMenu++;
    delay(200);
  }
  if(ddsMenu==2){
    DDSGeneratorMenu();    
    while(flag_key-digitalRead(key_up) && flag_key-digitalRead(key_down) && flag_key-digitalRead(key_ok)) {
      //PWM=pilaM[d];
      PWM=pgm_read_byte(&(waveformsTable[2][d]));
      pwmWrite(dds,PWM);
      d++;
      if(d==255) d=0; 
    }
    ddsMenu++;
    delay(200);
  }
  if(ddsMenu==3){
    DDSGeneratorMenu();    
    while(flag_key-digitalRead(key_up) && flag_key-digitalRead(key_down) && flag_key-digitalRead(key_ok)) {
      //PWM = RpilaM[d];
      PWM=pgm_read_byte(&(waveformsTable[3][d]));
      pwmWrite(dds,PWM);
      d++;
      if(d==255) d=0; 
    }
    ddsMenu++;
    delay(200);
  }
  if(ddsMenu==4){
    DDSGeneratorMenu();    
    while(flag_key-digitalRead(key_up) && flag_key-digitalRead(key_down) && flag_key-digitalRead(key_ok)) {
      //PWM = RpilaM[d];
      PWM=pgm_read_byte(&(waveformsTable[4][d]));
      pwmWrite(dds,PWM);
      d++;
      if(d==255) d=0; 
    }
    ddsMenu=0;
    delay(200);
  }
}
void TTL() {
  display.clearDisplay(); 
  display.setTextColor(WHITE, BLACK); 
  display.setCursor(0,0); display.println("   Terminal   ");
  display.setTextColor(BLACK);  
  display.setCursor(15,10); display.println("Hiz:");
  display.setTextColor(WHITE, BLACK); 
  display.setCursor(10,20); display.print("-");
  display.setTextColor(BLACK);
  display.println(" ");      
  display.print(speedTTL);
  display.println(" ");
  display.setTextColor(WHITE, BLACK);
  display.setCursor(69, 20);  display.println("+");
  display.setTextColor(BLACK);    
  display.setCursor(38,40); display.println("Ok");
  if(flag_key-!digitalRead(key_up))    STTL++;
  if(flag_key-!digitalRead(key_down))  STTL--;
  if(STTL>11) STTL=0;
  switch (STTL) {
    case  0: speedTTL = 300;    break;
    case  1: speedTTL = 1200;   break;
    case  2: speedTTL = 2400;   break;
    case  3: speedTTL = 4800;   break;
    case  4: speedTTL = 9600;   break;
    case  5: speedTTL = 19200;  break;
    case  6: speedTTL = 38400;  break;
    case  7: speedTTL = 57600;  break;
    case  8: speedTTL = 74880;  break;
    case  9: speedTTL = 115200; break;
    case 10: speedTTL = 230400; break;
    case 11: speedTTL = 250000; break;
  }
  if(flag_key-!digitalRead(key_ok)) {
    Serial.begin(speedTTL*(16/overclock));
    display.clearDisplay(); 
    delay(100);  
    display.display();
    int x=0;
    int y=0; 
    while(1){
      char incomingBytes;  
      if (Serial.available() > 0) {
        incomingBytes=Serial.read();
        display.setCursor(x,y);
        display.print(incomingBytes);
        display.display(); x=x+6;
        if(x==84){x=0;y=y+8;}
        if(y==48) {
          x=0;y=0;
          display.clearDisplay(); 
          delay(100);  
          display.display();
        }
      }         
    }
  }  
  display.display();  
  delay(200);
}  
void ReadADC() {
  if (razv>=6) ADCSRA = 0b11100010;
  if (razv==5) ADCSRA = 0b11100011; 
  if (razv==4) ADCSRA = 0b11100100; 
  if (razv==3) ADCSRA = 0b11100101; 
  if (razv==2) ADCSRA = 0b11100110; 
  if (razv<2)  ADCSRA = 0b11100111; 
  if (razv==0) {
    for(int i=0; i<700; i++) { 
      while ((ADCSRA & 0x10)==0);
      ADCSRA|=0x10;              
      delayMicroseconds(500);   
      mass[i]=ADCH;            
    }
  } else { // (razv>0)
    for(int i=0; i<700; i++) {    
      while ((ADCSRA & 0x10)==0); 
      ADCSRA|=0x10;              
      mass[i]=ADCH;              
    }
  }
}
void Ayar() {
  Set=0;delay(400);
  while (flag_key-digitalRead(key_ok)){
    display.clearDisplay();
    if (Set == 0) display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
    display.setCursor(0 , 0); display.println("Contrast:"); display.setTextColor(BLACK);
    display.setCursor(60 , 0); display.println(contrast);
    display.setCursor(0 , 10); display.println("B-Light :"); display.setTextColor(BLACK);
    if (BL == 1) { display.setCursor(60 , 10); display.println("ON ");} else
                 { display.setCursor(60 , 10); display.println("OFF");} display.display();
    if (flag_key-!digitalRead(key_up))   contrast++;
    if (flag_key-!digitalRead(key_down)) contrast--;
    if (contrast<30) contrast=30;
    if (contrast>70) contrast=70;
    display.setContrast(contrast); 
    delay(150);
  }
  Set=1; delay(400);
  while (flag_key-digitalRead(key_ok)){
    display.clearDisplay();
    display.setCursor(0 , 0); display.println("Contrast:");
    display.setCursor(60 , 0); display.println(contrast);
    if (Set == 1) display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
    display.setCursor(0 , 10); display.println("B-Light :"); display.setTextColor(BLACK);
    if (BL == 1) { display.setCursor(60 , 10); display.println("ON "); digitalWrite(Ekran, BL); } else
                 { display.setCursor(60 , 10); display.println("OFF"); digitalWrite(Ekran, BL);  }
    if (flag_key-!digitalRead(key_up))   BL=1;
    if (flag_key-!digitalRead(key_down)) BL=0;
    display.display();
    delay(150);
  }
  display.display();
  display.clearDisplay();
  EEPROM.write(1, contrast);
  EEPROM.write(2, BL);
  mainMenu=0;
  setup();
}
void setup() { 
  delay(300);  
  if ( EEPROM.read(cof) != cof ) {
      EEPROM.write(cof, cof);
      EEPROM.write(1, 60);
      EEPROM.write(2, 1);
  } else {
      contrast = EEPROM.read(1);
      BL       = EEPROM.read(2);
  }
  pinMode(Ekran, OUTPUT);
  digitalWrite(Ekran, BL);
  byte key_test=0;
  digitalWrite(key_up, HIGH); digitalWrite(key_down, HIGH); digitalWrite(key_ok, HIGH); 
  if (digitalRead(key_up))   key_test++; 
  if (digitalRead(key_down)) key_test++; 
  if (digitalRead(key_ok))   key_test++; 
  if (key_test>1) { 
    flag_key=0;   
  }
  else {
    digitalWrite(key_up, LOW); digitalWrite(key_down, LOW); digitalWrite(key_ok, LOW); 
    flag_key=1;
  }   
  display.begin();
  display.setContrast(contrast);  
  while(flag_key-digitalRead(key_ok))  {
    display.clearDisplay();
    if (mainMenu == 0)   display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
    display.setCursor(3 , 0); display.println("oscilloscope");
    if (mainMenu == 1)   display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
    display.setCursor(3 ,10); display.println("PWM-Generator");
    if (mainMenu == 2)  display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
    display.setCursor(3 ,20); display.println("DDS-Generator");
    if (mainMenu == 3)   display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
    display.setCursor(3 ,30); display.println("Terminal");
    if (mainMenu == 4)  display.setTextColor(WHITE, BLACK); else display.setTextColor(BLACK);
    display.setCursor(60 ,30); display.println("Set");
    display.setTextColor(BLACK);
    display.setCursor(12,40); display.print("Vbat=");
    display.print((analogRead(akb)*VCC)/1024);
    display.print("V"); 
    if (flag_key-!digitalRead(key_up)  ) { mainMenu--; delay(100); }
    if (flag_key-!digitalRead(key_down)) { mainMenu++; delay(100); }
    if (mainMenu == 0xFF) mainMenu = 4;
    if (mainMenu>4) mainMenu=0;
    delay(50);  
    display.display();
  }
  if (mainMenu==0) FreqCount.begin(1000);
  if (mainMenu==1) { InitTimersSafe(); bool success = SetPinFrequencySafe(led, freq); pwmWrite(led, PWM*2.55); }
  if (mainMenu==2) { InitTimersSafe(); bool success = SetPinFrequencySafe(dds,200000);}
  if (mainMenu==4) Ayar();
  delay(200);
}
void loop() {
  if(mainMenu==0) { 
    if(opornoe) ADMUX = 0b01100011;
           else ADMUX = 0b11100011;
    delay(5);
    if(pause==0) ReadADC();
    bool flagSINHRO=0;
    bool flagSINHRnull=0;
    for(int y=1; y<255 ;y++){
      if(flagSINHRO==0){if(mass[y]<SinU){flagSINHRnull=1;}}
      if(flagSINHRO==0){if(flagSINHRnull==1){if(mass[y]>SinU){flagSINHRO=1;sinhMASS=y;}}}}
    Vmax=0; 
    for(int y=1; y<255; y++) if(Vmax<mass[y]) Vmax=mass[y];
    display.clearDisplay();
    if(pause) {
      display.drawLine(prokr/8,8,prokr/8+6,8, BLACK); 
      display.drawLine(prokr/8,9,prokr/8+6,9, BLACK); 
      x=3;
      for(int y=prokr; y<prokr+80; y++) { 
        if(razv<7)  x++;
        if(razv==7) x+=2;
        if(razv==8) x+=3; 
        display.drawLine(x, x_line-mass[y]/7, x+1, x_line-mass[y+1]/7, BLACK);
        display.drawLine(x+1, x_line-mass[y]/7+1, x+2, x_line-mass[y+1]/7+1, BLACK); 
      }
    } else {     
      x=3;
      for(int y=sinhMASS; y<sinhMASS+80; y++) {
        if(razv<7)  x++; 
        if(razv==7) x+=2;
        if(razv==8) x+=3; 
        display.drawLine(x, x_line-mass[y]/7, x+1, x_line-mass[y+1]/7, BLACK);
        display.drawLine(x+1, x_line-mass[y]/7+1, x+2, x_line-mass[y+1]/7+1, BLACK);        
      }
      sinhMASS=0;
    }
    for(byte i=47;i>5;i=i-7) { display.drawPixel(0, i, BLACK); display.drawPixel(1, i, BLACK); display.drawPixel(2, i, BLACK); }
    for(byte i=47;i>5;i=i-3) { display.drawPixel(21,i, BLACK); display.drawPixel(42,i, BLACK); display.drawPixel(63,i, BLACK); }
    for(byte i=3;i<84;i=i+3) { display.drawPixel(i,33, BLACK); display.drawPixel(i,19, BLACK); }
    display.setCursor(0,0);
    if(oscMenu==0) {
      if(opornoe) display.print(Vmax*VCC/255,1); else display.print(Vmax*1.1/255,1);
      display.setTextColor(WHITE,BLACK);
      display.print("V");
      display.setTextColor(BLACK); 
      display.setCursor(26,0);display.print(razv);
      if(flag_key-!digitalRead(key_down) || flag_key-!digitalRead(key_up)) { opornoe=!opornoe; }
    }
    if(oscMenu==1) {
      if(opornoe) display.print(Vmax*VCC/255,1); else display.print(Vmax*1.1/255,1);display.print("V");
      display.setTextColor(WHITE, BLACK); 
      display.setCursor(26,0);display.print(razv);
      display.setTextColor(BLACK);
      if(flag_key-!digitalRead(key_down)) { razv=razv-1; if(razv==255) razv=0; }
      if(flag_key-!digitalRead(key_up)  ) { razv=razv+1; if(razv==9)   razv=8; }
    }
    if(oscMenu==4) {
      if(opornoe) display.print(Vmax*VCC/255,1); else display.print(Vmax*1.1/255,1);display.print("V");
      display.setCursor(26,0);display.print(razv);
      pause=1;
      if(flag_key-!digitalRead(key_down)) { prokr=prokr-10; if(prokr<0)   prokr=0;   }
      if(flag_key-!digitalRead(key_up)  ) { prokr=prokr+10; if(prokr>620) prokr=620; } 
      if(flag_key-!digitalRead(P)) {oscMenu=0;pause=0;delay(150);}   
    }
    if(oscMenu==3) {
      prokr=0;
      pause=0;
      if(opornoe) display.print(Vmax*VCC/255,1); else display.print(Vmax*1.1/255,1);display.print("V");
      display.setCursor(26,0);display.print(razv);
      if(flag_key-!digitalRead(key_down)) { SinU=SinU-20; if(SinU<20)  SinU=20;  }
      if(flag_key-!digitalRead(key_up)  ) { SinU=SinU+20; if(SinU>230) SinU=230; }   
      display.fillCircle(80,x_line-SinU/7, 2, BLACK); 
    }
    if(oscMenu==2) {
      if(opornoe) display.print(Vmax*VCC/255,1); else display.print(Vmax*1.1/255,1);display.print("V");
      display.setCursor(26,0);display.print(razv);
      display.drawLine(82,x_line-4,82,x_line, BLACK);
      display.drawLine(83,x_line-4,83,x_line, BLACK); 
      if(flag_key-!digitalRead(key_down)) {x_line++;if (x_line>47)x_line=47;}
      if(flag_key-!digitalRead(key_up)  ) {x_line--;if (x_line<25)x_line=27;}
    }
    if (flag_key-!digitalRead(key_ok)) { oscMenu++; if(oscMenu>3) { oscMenu=0; }}
    if (flag_key-!digitalRead(P)) oscMenu=4;
    if (FreqCount.available()) count = FreqCount.read();
    byte Frec1=0;
    long Frec=0;
    bool flagFrec1=0;
    bool flagFrec2=0;
    bool flagFrec3=0;
    for(int y=1; y<255; y++) {
      if(flagFrec1==0 && mass[y]<SinU) flagFrec2=1;
      if(flagFrec1==0 && flagFrec2==1 && mass[y]>SinU) { flagFrec1=1; Frec1=y; }
      if(flagFrec1==1 && mass[y]<SinU) flagFrec3=1;
      if(flagFrec3==1 && mass[y]>SinU) {
        if (razv>=6) Frec=1000000/((y-Frec1-1)*3.27);  //delitel 4
        if (razv==5) Frec=1000000/((y-Frec1)*3.27)/2;  //delitel 8
        if (razv==4) Frec=1000000/((y-Frec1)*3.27)/4;  //delitel 16
        if (razv==3) Frec=1000000/((y-Frec1)*3.27)/8;  //delitel 32
        if (razv==2) Frec=1000000/((y-Frec1)*3.27)/16; //delitel 64
        if (razv==2) Frec=1000000/((y-Frec1)*3.27)/32; //delitel 128
        if (razv==1) Frec=1000000/((y-Frec1)*3.27)/32; //delitel 128
        if (razv==0) Frec=1000000/((y-Frec1)*500);     //delitel 128
        flagFrec1=0; flagFrec3=0;
      }
    }
    if(opornoe==1) {
      if((Vmax*VCC/255)>2.5) countX=count*(overclock/16.0); 
      if((Vmax*VCC/255)<2.5) countX=Frec*(overclock/16.0);
    } else countX=Frec*(overclock/16.0);
    if(countX<1000) { display.setCursor(34,0); display.print(countX); display.print("Hz"); }
    if(countX>=1000) { float countXK=countX/1000.0; display.setCursor(34,0);display.print(countXK,1); display.print("KHz"); }
    display.setCursor(0,40); 
    if(opornoe) display.print(VCC,1); else display.print("1.1");
    display.print("V");
    delay(150);  
    display.display();
  }
  if(mainMenu==1) Generator();
  if(mainMenu==2) DDSGenerator();
  if(mainMenu==3) TTL();
}
