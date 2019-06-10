#include <ADC.h>

#define SAMPLE_NO 2000

const int readPin_1 = A2;
const int readPin_2 = A0;
int j;
int k;
bool flag;
int buffer_1[SAMPLE_NO];
int buffer_2[SAMPLE_NO+5];

ADC *adc = new ADC();

void setup() {

  //Read pin is A3 (pin 17) on Teensy 3.6
  //Oscilloscope signal generator attached to A3 and Analog GND
  pinMode(readPin_1, INPUT);
  pinMode(readPin_2, INPUT);
  delay(6000);
  Serial.begin(2000000);
  //Serial.println("Teeny beginning data acquisition...");

  //Configure ADC_0 for Continuous measurement
  adc->setAveraging(1, ADC_1);
  adc->setResolution(16, ADC_1);
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_1);
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED, ADC_1);
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_1);
  adc->startContinuous(readPin_1, ADC_1);
  adc->setAveraging(1, ADC_0);
  adc->setResolution(16, ADC_0);
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_0);
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED, ADC_0);
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0);
  adc->startContinuous(readPin_2, ADC_0);
  delay(1000);
}

//Start measurement timer
elapsedMicros dt;
uint16_t value;

void loop() {
  adc->startContinuous(readPin_1, ADC_1);
  delay(1000);
  adc->startContinuous(readPin_2, ADC_0);
  delay(1000);
  j = 0;
  k = 0;
  dt = 0;

    //Fill buffer with ADC_0 output values
  while (j < SAMPLE_NO) {
    if(adc->isComplete(ADC_1)){
        value = (uint16_t) adc->analogReadContinuous(ADC_1);
        buffer_1[j] = value;
        j++;
    }
    if(adc->isComplete(ADC_0)){
        value = (uint16_t) adc->analogReadContinuous(ADC_0);
        buffer_2[k] = value;
        k++;
    }
  }
  //Serial.println(dt);

  adc->stopContinuous(ADC_0);
  adc->stopContinuous(ADC_1);

  //Convert buffer to float values and print to Serial
  //Serial.print("Measurement time [us]: ");
  //Serial.println(dt);
  //Serial.println("Buffer Values [V]: ");

  dt = 0;
  for (int i = 0; i<SAMPLE_NO; i++) {
    Serial.print(buffer_1[i]);
    Serial.print(",");
    Serial.print(buffer_2[i]);
    Serial.print("\n");
  }
  //Serial.println(dt);
  delay(1000);

  }
