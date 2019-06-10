/////////////////////////////////////////////////
/////////           LIBRARIES           /////////
/////////////////////////////////////////////////

#include "Arduino.h"
#include "ADC.h"
#include "RingBuffer.h"
#include <IntervalTimer.h>

/////////////////////////////////////////////////
/////////       PIN SETUP         /////////
/////////////////////////////////////////////////

const int readPin0 = A0;
const int period0 = 150; // us

const int readPin1 = A15;
const int period1 = 150; // us

const int timer_interval = 30; //us
const int readPeriod = 500; // us

ADC *adc = new ADC(); // adc object

IntervalTimer timer0, timer1; // timers

RingBuffer *buffer0 = new RingBuffer; // buffers to store the values
RingBuffer *buffer1 = new RingBuffer;


int startTimerValue0 = 0, startTimerValue1 = 0;

// This function will be called with the desired frequency
// start the measurement
// in my low-res oscilloscope this function seems to take 1.5-2 us.
void timer0_callback(void) {
    digitalWriteFast(1, HIGH);
    adc->startSingleRead(readPin0, ADC_0);
    digitalWriteFast(1, LOW);
}

void timer1_callback(void) {
  digitalWriteFast(5, HIGH);
  adc->startSingleRead(readPin1, ADC_0);
  digitalWriteFast(5, LOW);
}

// This function will be called with the desired frequency
// start the measurement

// when the measurement finishes, this will be called
// first: see which pin finished and then save the measurement into the correct buffer
void adc0_isr() {

    uint8_t pin = ADC::sc1a2channelADC0[ADC0_SC1A&ADC_SC1A_CHANNELS]; // the bits 0-4 of ADC0_SC1A have the channel

    // add value to correct buffer
    if(pin==readPin0) {
      digitalWriteFast(3, HIGH);
      buffer0->write((uint16_t) adc->readSingle());
      digitalWriteFast(3, LOW);
    }
    else if(pin==readPin1){
      digitalWriteFast(7, HIGH);
      buffer1->write((uint16_t) adc->readSingle());
      digitalWriteFast(7, LOW);
    }
    else { // clear interrupt anyway
        adc->readSingle();
    }

    // restore ADC config if it was in use before being interrupted by the analog timer
    if (adc->adc0->adcWasInUse) {
      digitalWriteFast(LED_BUILTIN, HIGH);
        // restore ADC config, and restart conversion
        adc->adc0->loadConfig(&adc->adc0->adc_config);
        // avoid a conversion started by this isr to repeat itself
        adc->adc0->adcWasInUse = false;
      digitalWriteFast(LED_BUILTIN, LOW);
    }
}


void setup() {

    //OSCILOSCOPE
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(1, OUTPUT); // timer0 starts a measurement
    pinMode(3, OUTPUT); // timer1 starts a measurement
    pinMode(5, OUTPUT); // adc0_isr, measurement finished for readPin0
    pinMode(7, OUTPUT); // adc0_isr, measurement finished for readPin1

    pinMode(readPin0, INPUT);
    pinMode(readPin1, INPUT);
    Serial.begin(115200);
    delay(1000);

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0);
    adc->setAveraging(4); // set number of averages
    adc->setResolution(12); // set bits of resolution
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed
    adc->enableInterrupts(ADC_0);
    delay(500);


    // start the timers, if it's not possible, startTimerValuex will be false
    //startTimerValue0 = timer0.begin(timer0_callback, period0);
    //delayMicroseconds(timer_interval);
    //startTimerValue1 = timer1.begin(timer1_callback, period1);
    //delayMicroseconds(timer_interval);
}

int value = 0;
char c=0;
int max_value = 0;
elapsedMicros wait = 0;
int time_s;

void loop() {

  startTimerValue0 = timer0.begin(timer0_callback, period0);
  delayMicroseconds(timer_interval);
  startTimerValue1 = timer1.begin(timer1_callback, period1);
  delayMicroseconds(timer_interval);

  while(1){
    if(!buffer0->isEmpty()) { // read the values in the buffer
        //Serial.print("pin 0: ");
        //if (buffer0->read() > max_value){
          //max_value = buffer0->read();
        //}
        Serial.print(buffer0->read());
        //Serial.print(" ");
        //Serial.print(max_value);
        //Serial.println("New value!");
    }

    if(!buffer1->isEmpty()) { // read the values in the buffer
      //Serial.print("pin 1: ");
        Serial.print(",");
        Serial.println(buffer1->read());
        //Serial.println("New value!");
    }
    //time_s = wait;
    //Serial.println(" ");
    //Serial.println(time_s);
    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
    //wait = 0;
    delayMicroseconds(readPeriod);
  }
}
