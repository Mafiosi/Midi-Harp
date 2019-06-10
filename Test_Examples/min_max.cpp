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

  //Configure ADC_0 for Continuous measurement
  adc->setAveraging(1, ADC_0);
  adc->setResolution(16, ADC_0);
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED, ADC_0);
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED, ADC_0);
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0);
  adc->startContinuous(readPin_1, ADC_0);
  delay(1000);
}

//Start measurement timer
elapsedMicros dt;
uint16_t value = 0, max_value = 0, min_value = 0;
bool flag_get = 0;

// Data Collection Values

void loop() {
  adc->startContinuous(readPin_1, ADC_0);
  j = 0;
  dt = 0;
  min_value =65535;
  max_value = 0;
  flag_get = 0;

  //Fill buffer with ADC_0 output values
  while (j < SAMPLE_NO) {
    if(adc->isComplete(ADC_0)){
      value = (uint16_t) adc->analogReadContinuous(ADC_0);
      if(value > 400 && flag_get == 0){
        flag_get = 1;
      }
    }
    if(flag_get == 1){
      buffer_1[j] = value;
      j++;
    }
  }
  //Serial.println(dt);

  adc->stopContinuous(ADC_0);
  //Convert buffer to float values and print to Serial
  //Serial.print("Measurement time [us]: ");
  //Serial.println(dt);
  //Serial.println("Buffer Values [V]: ");

  dt = 0;
  for (int i = 0; i<SAMPLE_NO; i++) {
    if(max_value < buffer_1[i]){
      max_value = buffer_1[i];
    }
    if(min_value > buffer_1[i]){
      min_value = buffer_1[i];
    }
  }
  Serial.println(min_value);
  Serial.println(max_value);
  }
