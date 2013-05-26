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
    analogWrite(red_pin, 0);

    pinMode(yellow_pin,OUTPUT);
    analogWrite(yellow_pin, 0);

    pinMode(green_pin, OUTPUT);
    analogWrite(green_pin, 0);

    pinMode(blue_pin, OUTPUT);
    analogWrite(blue_pin, 0);
}

//------------------------------------------------------------------------------

void loop() {
    uint8_t i;
    uint16_t val;
    char buf[40];

delay(10000);
Serial.println("Starting up...");

    do {
        for (i = 0; i < 128; i++) {
            val = analogRead(adc_pin);
            // Convert 10-bit unsigned reading with center at 0x200 (512) to -128..+127
            data[i] = (val >> 2) - 0x80;
            im[i] = 0;
        }

        fix_fft(data, im, 7, 0);

        chan_a = 0;
        chan_b = 0;
        chan_c = 0;
        chan_d = 0;

        // Merge 64 frequency bands into 4 (channels A..D)

        for (i = 0; i < 64; i++) {
            val = sqrt(data[i] * data[i] + im[i] * im[i]); 

            if (i < 16) {
                chan_a += val;
            }
            else if (i < 32) {
                chan_b += val;
            }
            else if (i < 48) {
                chan_c += val;
            }
            else {
                chan_d += val;
            }
        }

        // Scale summed channel data down to 8 bits
        // Divide each by 16, since 16 8-bit data points were summed into each channel

//sprintf(buf, "A:%03X B:%03X C:%03X D:%03X", chan_a, chan_b, chan_c, chan_d);
//Serial.println(buf);

        chan_a >>= 4;
        chan_b >>= 4;
        chan_c >>= 4;
        chan_d >>= 4;

        // Send channel values to PWM outputs

        setPWM(red_pin, chan_a);
        setPWM(yellow_pin, chan_b);
        setPWM(green_pin, chan_c);
        setPWM(blue_pin, chan_d);
    } while (1);

}