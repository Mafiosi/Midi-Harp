#include <ADC.h>
#include <Arduino.h>

#define SAMPLE_NO 50000

const int readPin_1 = A0;
int j;
bool flag;
int buffer_1[SAMPLE_NO];

ADC *adc = new ADC();

void setup() {

  //Read pin is A3 (pin 17) on Teensy 3.6
  //Oscilloscope signal generator attached to A3 and Analog GND
  pinMode(readPin_1, INPUT);
  Serial.begin(115200);
  delay(4000);
  //Serial.println("Teeny beginning data acquisition...");
}

//Start measurement timer
elapsedMicros wait = 0;
int unsigned wait_time = 250;
uint16_t value = 0, max_value = 0, min_value = 0;
bool flag_get = 0;

// Data Collection Values

void loop() {
  j = 0;
  wait = 0;
  min_value =65535;
  max_value = 0;
  flag_get = 0;

  //Fill buffer with ADC_0 output values
  while (j < SAMPLE_NO) {
    if(wait >= wait_time){
      value = analogRead(0);
      wait = 0;
      if(value > 200 && flag_get == 0){
        flag_get = 1;
      }
      if(flag_get == 1){
        buffer_1[j] = value;
        j++;
      }
    }
  }
  //Serial.println(dt);

  //Convert buffer to float values and print to Serial
  //Serial.print("Measurement time [us]: ");
  //Serial.println(dt);
  //Serial.println("Buffer Values [V]: ");

  for (int i = 0; i<SAMPLE_NO; i++) {
    if(max_value < buffer_1[i]){
      max_value = buffer_1[i];
    }
    if(min_value > buffer_1[i]){
      min_value = buffer_1[i];
    }
    Serial.print(buffer_1[i]);
    Serial.print("\n");
  }
  Serial.println(min_value);
  Serial.println(max_value);
  }
