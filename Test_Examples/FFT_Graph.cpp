#include "arduinoFFT.h"

#define SAMPLES 1024               //Must be a power of 2
#define SAMPLING_FREQUENCY 50000  //Hz
#define REFRESH_RATE 10           //Hz
#define ARDUINO_IDE_PLOTTER_SIZE 500

arduinoFFT FFT = arduinoFFT();

unsigned long sampling_period_us;
unsigned long useconds_sampling;

unsigned long refresh_period_us;
unsigned long useconds_refresh;

double vReal[SAMPLES];
double vImag[SAMPLES];

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
      vImag[0] = 0;
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
    vImag[i] = 0;

  }

  /*FFT*/
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_BLACKMAN, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  /*PRINT RESULTS*/
  for(int i=0; i<(SAMPLES/2); i++){
    Serial.print((int)vReal[i]);
    Serial.print("\n");
  }

  Serial.print(0);
  Serial.print("\n");

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
