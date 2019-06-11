/////////////////////////////////////////////////
/////////           LIBRARIES           /////////
/////////////////////////////////////////////////

#include "Arduino.h"
#include "ADC.h"
#include "RingBuffer.h"
#include <IntervalTimer.h>

/////////////////////////////////////////////////
/////////           PIN SETUP           /////////
/////////////////////////////////////////////////

const int readPin0 = A12; // RED
const int readPin1 = A0;  // GREY
const int readPin2 = A15; // BLACK

/////////////////////////////////////////////////
/////////     TUNABLE CONFIGURATION     /////////
/////////////////////////////////////////////////

// MIDI CONTROL VARIABLES
const byte MIDI_CHANNEL = 1;    // MIDI CHANNEL NUMBER
const byte MIDI_n_inputs = 3;   // HOW MANY INPUTS WILL BE READ

// GENERIC CONTROL VARIABLES
const bool Serial_print = 1;                  // PRINT MESSAGES TO SERIAL
const bool debug_print = 0;                   // Print Debug Messages
const byte input_pitch[MIDI_n_inputs] = {64, 60, 56}; // PITCH ASSOCIATED WITH EACH INPUT

/////////////////////////////////////////////////
/////////    PERMANENT  CONFIGURATION   /////////
/////////////////////////////////////////////////

// TIMER CONTROL VARIABLES
const int period0 = 150; // RED us
const int period1 = 150; // GREY us
const int period2 = 150; // BLACK us

// GENERAL
const int timer_interval = 40; //us
const int readPeriod = 500; // us

//MIDI CONTROL VARIABLES

/////////////////////////////////////////////////
/////////     VARIABLE DECLARATION      /////////
/////////////////////////////////////////////////

// ADC
ADC *adc = new ADC(); // adc object

// BUFFER TO STORE VARIABLES
RingBuffer *buffer0 = new RingBuffer; // RED
RingBuffer *buffer1 = new RingBuffer; // GREY
RingBuffer *buffer2 = new RingBuffer; // BLACK

// TIMER VARIBALES
IntervalTimer timer0, timer1, timer2; // timers (RED, GREY, BLACK)
int startTimerValue0 = 0, startTimerValue1 = 0, startTimerValue2 = 0;
const int timer_MIDI_OFF = 500000;                                // MINIMUM VALUE TO WAIT TO SEND MIFI OFF AFTER INITIAL DELAY (us)


// GENERIC CONTROL VARIABLES
const uint16_t bitresolution = 4095;                                  // BIT RESOLUTION (12 bits)

// MIDI CONTROL VARIABLES
const uint16_t threshold_MIN[MIDI_n_inputs] = {530, 530, 530};        // MINIMUM VALUE TO ACCEPT AS MESSAGE (0.3 Volt)
const uint16_t threshold_MAX[MIDI_n_inputs] = {4095, 4095, 4095};     // MAXIMUM VALUE TO ACCEPT (3.3 Volt)
const uint8_t max_samples = 10;                                       // HOW MANY SAMPLES TO COLLECT IN INIT
const uint8_t threshold_MIN_OFF[MIDI_n_inputs] = {25, 25, 25};        // MINIMUM VALUE TO SEND MIDI NOTE OFF (0.02 Volt)
const uint8_t max_samples_off = 10;                                   // HOW MANY SAMPLES TO SEND MIDI OFF

// CONTROL VARIABLES
byte unsigned input_state[MIDI_n_inputs];
uint16_t max_value[MIDI_n_inputs];
uint16_t value_counter[MIDI_n_inputs];
elapsedMicros single_timer[MIDI_n_inputs];
bool flag_MIDI_OFF[MIDI_n_inputs];

/////////////////////////////////////////////////
/////////          INTERRUPTS           /////////
/////////////////////////////////////////////////

void timer0_callback(void) {
  digitalWriteFast(1, HIGH);
  adc->startSingleRead(readPin0, ADC_1);
  digitalWriteFast(1, LOW);
}

void timer1_callback(void) {
  digitalWriteFast(5, HIGH);
  adc->startSingleRead(readPin1, ADC_0);
  digitalWriteFast(1, LOW);
}

void timer2_callback(void) {
  digitalWriteFast(9, HIGH);
  adc->startSingleRead(readPin2, ADC_0);
  digitalWriteFast(1, LOW);
}

void adc0_isr() {

    uint8_t pin = ADC::sc1a2channelADC0[ADC0_SC1A&ADC_SC1A_CHANNELS]; // the bits 0-4 of ADC0_SC1A have the channel

    // RED PIN
    if(pin==readPin0) {
      digitalWriteFast(3, HIGH);
      if(input_state[0] == 0){
        if((uint16_t) adc->adc0->readSingle() > threshold_MIN[0]){
          max_value[0] = adc->adc0->readSingle();
          input_state[0] = 1;
        }
      } else if(input_state[0] == 1 && value_counter[0] < max_samples){
          value_counter[0]++;
          if((uint16_t) adc->adc0->readSingle() > max_value[0]){
            max_value[0] = adc->adc0->readSingle();
          }
      } else if(input_state[0] == 2 && flag_MIDI_OFF[0] == 0){
          if((uint16_t) adc->adc0->readSingle() < threshold_MIN_OFF[0]){
            value_counter[0]++;
          } else{
            value_counter[0] = 0;
          }
          if(value_counter[0] > max_samples_off){
            flag_MIDI_OFF[0] = 1;
          }
      }
      digitalWriteFast(3, LOW);
    }

    // GREY PIN
    else if(pin==readPin1) {
      digitalWriteFast(7, HIGH);
      if(input_state[1] == 0){
        if((uint16_t) adc->adc0->readSingle() > threshold_MIN[1]){
          max_value[1] = adc->adc0->readSingle();
          input_state[1] = 1;
        }
      } else if(input_state[1] == 1 && value_counter[1] < max_samples){
          value_counter[1]++;
          if((uint16_t) adc->adc0->readSingle() > max_value[1]){
            max_value[1] = adc->adc0->readSingle();
          }
      } else if(input_state[1] == 2 && flag_MIDI_OFF[1] == 0){
          if((uint16_t) adc->adc0->readSingle() < threshold_MIN_OFF[1]){
            value_counter[1]++;
          } else{
            value_counter[1] = 0;
          }
          if(value_counter[1] > max_samples_off){
            flag_MIDI_OFF[1] = 1;
          }
      }
      digitalWriteFast(7, LOW);
    }

    else {
        adc->adc0->readSingle();  // clear interrupt anyway
    }

    // restore ADC config if it was in use before being interrupted by the analog timer
    if (adc->adc0->adcWasInUse) {
      digitalWriteFast(LED_BUILTIN, HIGH);
      adc->adc0->loadConfig(&adc->adc0->adc_config);  // restore ADC config, and restart conversion
      adc->adc0->adcWasInUse = false; // avoid a conversion started by this isr to repeat itself
      digitalWriteFast(LED_BUILTIN, LOW);
    }
}

void adc1_isr() {

    digitalWriteFast(11, HIGH);
    if(input_state[2] == 0){
      if((uint16_t) adc->adc1->readSingle() > threshold_MIN[2]){
        max_value[2] = adc->adc1->readSingle();
        input_state[2] = 1;
      }
    } else if(input_state[2] == 1 && value_counter[2] < max_samples){
        value_counter[2]++;
        if((uint16_t) adc->adc1->readSingle() > max_value[2]){
          max_value[2] = adc->adc1->readSingle();
        }
    } else if(input_state[2] == 2 && flag_MIDI_OFF[2] == 0){
        if((uint16_t) adc->adc1->readSingle() < threshold_MIN_OFF[2]){
          value_counter[2]++;
        } else{
          value_counter[2] = 0;
        }
        if(value_counter[2] > max_samples_off){
          flag_MIDI_OFF[2] = 1;
        }
    }
    else {
        adc->adc1->readSingle();  // clear interrupt anyway
    }
    digitalWriteFast(11, LOW);
    // restore ADC config if it was in use before being interrupted by the analog timer
    if (adc->adc1->adcWasInUse) {
        adc->adc1->loadConfig(&adc->adc1->adc_config);  // restore ADC config, and restart conversion
        adc->adc1->adcWasInUse = false; // avoid a conversion started by this isr to repeat itself
    }
}

/////////////////////////////////////////////////
/////////     FUNCTIONS DECLARATION     /////////
/////////////////////////////////////////////////

void MIDI_sendON(byte pitch, byte velocity){
  usbMIDI.sendNoteOn(pitch, velocity, MIDI_CHANNEL);
}

void MIDI_sendOFF(byte pitch){
  usbMIDI.sendNoteOff(pitch, 0, MIDI_CHANNEL);
}

byte normalizevelocity(int value, byte pin){
  byte velocity = value*127/threshold_MAX[pin];
  return velocity;
}

/////////////////////////////////////////////////
/////////    SERIAL COMMUNICATION       /////////
/////////////////////////////////////////////////

// SETUP PROCESS
void setup()
{
  // RESET VARIABLES
  for(byte unsigned temp = 0; temp < MIDI_n_inputs; temp++){
    input_state[temp] = 0;
    max_value[temp] = 0;
    value_counter[temp] = 0;
    flag_MIDI_OFF[temp] = 0;
  }

  // SET PIN MODE
  pinMode(readPin0, INPUT);
  pinMode(readPin1, INPUT);
  pinMode(readPin2, INPUT);
  Serial.begin(115200);
  delay(1000);

  // Debug
  pinMode(1, OUTPUT); // timer0 starts a measurement
  pinMode(3, OUTPUT); // timer1 starts a measurement
  pinMode(5, OUTPUT); // adc0_isr, measurement finished for readPin0
  pinMode(7, OUTPUT); // adc0_isr, measurement finished for readPin1
  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);

  //ADC_0
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0);
  adc->setAveraging(4, ADC_0); // set number of averages
  adc->setResolution(12, ADC_0); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED, ADC_0); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_0); // change the sampling speed
  adc->enableInterrupts(ADC_0);
  delay(500);

  //ADC_1
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_1);
  adc->setAveraging(4, ADC_1); // set number of averages
  adc->setResolution(12, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED, ADC_1); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_1); // change the sampling speed
  adc->enableInterrupts(ADC_1);
  delay(500);

  // START TIMER
  startTimerValue0 = timer0.begin(timer0_callback, period0);
  delayMicroseconds(timer_interval);
  startTimerValue1 = timer1.begin(timer1_callback, period1);
  delayMicroseconds(timer_interval);
  startTimerValue2 = timer2.begin(timer2_callback, period2);
  delayMicroseconds(timer_interval);
}

/////////////////////////////////////////////////
/////////           MAIN LOOP           /////////
/////////////////////////////////////////////////
void loop(){

  // MAIN LOOP
  while(1){

    // READ EACH BUFFER
    for(byte unsigned pin = 0; pin < MIDI_n_inputs; pin++){

      // STATE 0 - COLLECT SAMPLE AND WAIT FOR SIGNAL
      // ALL DONE BY INTERRUPTS

      // STATE 1 - SEND MIDI ON WHEN READY
      if(input_state[pin] == 1){
        if(value_counter[pin] >= max_samples){
          MIDI_sendON(input_pitch[pin], normalizevelocity(max_value[pin], pin));
          input_state[pin] = 2;
          value_counter[pin] = 0;
          single_timer[pin] = 0;
        }
      }

      // STATE 2 - SEND MIDI OFF BY INTERRUPT OR TIMER
      if(input_state[pin] == 2){
        if(single_timer[pin] >= timer_MIDI_OFF || flag_MIDI_OFF[pin] == 1){
          MIDI_sendOFF(input_pitch[pin]);
          input_state[pin] = 0;
          flag_MIDI_OFF[pin] = 0;
          value_counter[pin] = 0;
        }
      }
    }
    delayMicroseconds(readPeriod);
  }
}
