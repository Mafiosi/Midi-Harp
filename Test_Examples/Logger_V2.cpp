#include "Arduino.h"


#define SAMPLES 1000               //Must be a power of 2
#define SAMPLING_FREQUENCY 300  //Hz
#define REFRESH_RATE 10           //Hz
#define ARDUINO_IDE_PLOTTER_SIZE 500


unsigned long sampling_period_us;
unsigned long useconds_sampling;

unsigned long refresh_period_us;
unsigned long useconds_refresh;

double vReal[SAMPLES];

uint8_t analogpin = A0;
bool flag_run = 0;

void setup() {
  Serial.begin(115200);

  sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
  refresh_period_us = round(1000000*(1.0/REFRESH_RATE));

  pinMode(analogpin, INPUT);
}

void loop() {
  useconds_refresh = micros();

  /*SAMPLING*/
  while(1){
    useconds_sampling = micros();
    double test_value = analogRead(analogpin);

    if(test_value >= 200 && test_value <= 1023){
      vReal[0] = test_value;
      break;
    }

    while(micros() < (useconds_sampling + sampling_period_us)){
      //wait...
    }
  }

  for(int i=1; i<SAMPLES; i++)
  {

    while(micros() < (useconds_sampling + sampling_period_us)){
      //wait...
    }

    useconds_sampling = micros();

    vReal[i] = analogRead(analogpin);

  }


  /*PRINT RESULTS*/
  for(int i=0; i<(SAMPLES); i++){
    Serial.print((int)vReal[i]);
    Serial.print("\n");
  }

  while(1){
    useconds_sampling = micros();
    double test_value = analogRead(analogpin);

    if(test_value == 0){
      break;
    }

    while(micros() < (useconds_sampling + sampling_period_us)){
      //wait...
    }
  }
}
