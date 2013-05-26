#include <stdint.h>
#include "fix_fft.h"

const uint8_t red_pin = 10;
const uint8_t yellow_pin = 9;
const uint8_t green_pin = 5;
const uint8_t blue_pin = 4;
const uint8_t adc_pin = 18;

uint16_t chan_a;
uint16_t chan_b;
uint16_t chan_c;
uint16_t chan_d;

char im[128];
char data[128];

//------------------------------------------------------------------------------

void setPWM(uint8_t pin, uint8_t value){
    analogWrite(pin, 0xFF - value);
}

//------------------------------------------------------------------------------

void setup() {
    Serial.begin(38400);

    pinMode(adc_pin, INPUT);

    pinMode(red_pin, OUTPUT);
    analogWrite(red_pin, 0x00);

    pinMode(yellow_pin,OUTPUT);
    analogWrite(yellow_pin, 0xFF);

    pinMode(green_pin, OUTPUT);
    analogWrite(green_pin, 0xFF);

    pinMode(blue_pin, OUTPUT);
    analogWrite(blue_pin, 0x00);
}

//------------------------------------------------------------------------------

void loop() {
    uint8_t i;
    uint16_t val;
    char buf[40];

Serial.println("Starting up...");

    do {
        for (i = 0; i < 128; i++) {
            val = analogRead(adc_pin);
            // Convert 10-bit unsigned reading with center at 0x200 (512) to -128..+127
            data[i] = (val >> 2) - 0x80;
            im[i] = 0;
        }

        fix_fft(data, im, 7, 0);
/*
        chan_a = 0;
        chan_b = 0;
        chan_c = 0;
        chan_d = 0;
*/
        // Simple decay filter

        chan_a -= 16;
        if (chan_a & 0x8000) chan_a = 0;
        chan_b -= 16;
        if (chan_b & 0x8000) chan_b = 0;
        chan_c -= 16;
        if (chan_c & 0x8000) chan_c = 0;
        chan_d -= 16;
        if (chan_d & 0x8000) chan_d = 0;

        // Merge 64 frequency bands into 4 (channels A..D)

        for (i = 1; i < 64; i++) {
//          val = sqrt(data[i] * data[i] + im[i] * im[i]); 
            val = (data[i] * data[i] + im[i] * im[i]) >> 3;

            if (i < 10) {
                if (val > chan_a) chan_a = val;
            }
            else if (i < 20) {
                if (val > chan_b) chan_b = val;
            }
            else if (i < 50) {
                if (val > chan_c) chan_c = val;
            }
            else {
                if (val > chan_d) chan_d = val;
            }
        }

        // Bound channel data to 8-bit maxint

        if (chan_a > 0xFF) chan_a = 0xFF;
        if (chan_b > 0xFF) chan_b = 0xFF;
        if (chan_c > 0xFF) chan_c = 0xFF;
        if (chan_d > 0xFF) chan_d = 0xFF;

        // Send channel values to PWM outputs

        setPWM(red_pin, chan_a);
        setPWM(yellow_pin, chan_b);
        setPWM(green_pin, chan_c);
        setPWM(blue_pin, chan_d);
    } while (1);

}