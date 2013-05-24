#include "fix_fft.h"

static const int redPin = 10;
static const int yellowPin = 9;
static const int greenPin = 12;
static const int bluePin = 14;
static const int pin_adc = 19;

char im[128];
char data[128]; 

int chanA = 255;
int chanB = 255;
int chanC = 255;
int chanD = 255;

void setup(){
  pinMode(pin_adc, INPUT);
  pinMode(redPin,OUTPUT);
  analogWrite(redPin, 255 - chanA);
  
  pinMode(yellowPin,OUTPUT);
  analogWrite(yellowPin, 255 - chanB);
  
  pinMode(greenPin,OUTPUT);
  analogWrite(greenPin, 255 - chanC);

  pinMode(bluePin,OUTPUT);
  analogWrite(bluePin, 255 - chanD);
}

void setPin(int pin, int value){
  analogWrite(pin, 255 - value);
}

void loop(){
  int static i = 0;
  static long tt;
  int val;

  
   
   if (millis() > tt){
      if (i < 128){
        val = analogRead(pin_adc);
        data[i] = val / 4 - 128;
        im[i] = 0;
        i++;   
        
      }
      else{
        //this could be done with the fix_fftr function without the im array.
        fix_fft(data,im,7,0);
        // I am only interessted in the absolute value of the transformation
        for (i=0; i< 64;i++){
           data[i] = sqrt(data[i] * data[i] + im[i] * im[i]); 
        }
        chanA = 0;
        chanB = 0;
        chanC = 0;
        chanD = 0;
        for (i=0; i< 64;i++){

          if (i < 16) {
            chanA = chanA + data[i];
          }
          if (i > 16 && i < 32) {
            chanB = chanB + data[i];
          }
          if (i > 32 && i < 48) {
            chanC = chanC + data[i];
          }
          if (i >= 48) {
            chanD = chanD + data[i];
          }
          chanA = chanA / 16;
          chanB = chanB / 16;
          chanC = chanC / 16;
          chanD = chanD / 16;
          setPin(redPin, chanA);
          setPin(yellowPin, chanB);
          setPin(greenPin, chanC);
          setPin(bluePin, chanD);
          }
        //do something with the data values 1..64 and ignore im
        //show_big_bars(data,0);
      }
    
    tt = millis();
   }
}
