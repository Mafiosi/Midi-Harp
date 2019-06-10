/*
    This example shows how to use the IntervalTimer library and the ADC library in the Teensy 3.0/3.1
    The three important objects are: the ADC, the (one or more) IntervalTimer and the same number of RingBuffer.
    The ADC sets up the internal ADC, you can change the settings as usual. Enable interrupts.
    The IntervalTimer (timerX) executes a function every 'period' microseconds.
    This function, timerX_callback, starts the desired ADC measurement, for example:
        startSingleRead, startSingleDifferential, startSynchronizedSingleRead, startSynchronizedSingleDifferential
    you can use the ADC0 or ADC1 (only for Teensy 3.1).
    When the measurement is done, the adc_isr is executed:
        - If you have more than one timer per ADC module you need to know which pin was measured.
        - Then you store/process the data
        - Finally, if you have more than one timer you can check whether the last measurement interruped a
          a previous one (using adc->adcX->adcWasInUse), and restart it if so.
          The settings of the interrupted measurement are stored in the adc->adcX->adc_config struct.
*/


#include <Arduino.h>
#include "ADC.h"
#include "RingBuffer.h"
// and IntervalTimer
#include <IntervalTimer.h>

const int ledPin = LED_BUILTIN;

const int readPin0 = A0;
const int period0 = 60; // us


const int readPeriod = 1000; // us

ADC *adc = new ADC(); // adc object

IntervalTimer timer0; // timers

RingBuffer *buffer0 = new RingBuffer; // buffers to store the values

int startTimerValue0 = 0, startTimerValue1 = 0;

// This function will be called with the desired frequency
// start the measurement
// in my low-res oscilloscope this function seems to take 1.5-2 us.
void timer0_callback(void) {


    adc->startSingleRead(readPin0, ADC_0); // also: startSingleDifferential, analogSynchronizedRead, analogSynchronizedReadDifferential

    //digitalWriteFast(ledPin+1, !digitalReadFast(ledPin+1));

}

// This function will be called with the desired frequency
// start the measurement

// when the measurement finishes, this will be called
// first: see which pin finished and then save the measurement into the correct buffer
void adc0_isr() {

    uint8_t pin = ADC::sc1a2channelADC0[ADC0_SC1A&ADC_SC1A_CHANNELS]; // the bits 0-4 of ADC0_SC1A have the channel

    // add value to correct buffer
    if(pin==readPin0) {
        buffer0->write((uint16_t) adc->readSingle());
    } else { // clear interrupt anyway
        adc->readSingle();
    }

    // restore ADC config if it was in use before being interrupted by the analog timer
    if (adc->adc0->adcWasInUse) {
        // restore ADC config, and restart conversion
        adc->adc0->loadConfig(&adc->adc0->adc_config);
        // avoid a conversion started by this isr to repeat itself
        adc->adc0->adcWasInUse = false;
    }


    //digitalWriteFast(ledPin+2, !digitalReadFast(ledPin+2));

}


void setup() {

    pinMode(ledPin, OUTPUT); // led blinks every loop

    pinMode(ledPin+1, OUTPUT); // timer0 starts a measurement
    pinMode(ledPin+3, OUTPUT); // adc0_isr, measurement finished for readPin0

    pinMode(readPin0, INPUT);

    Serial.begin(115200);

    delay(1000);

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(1); // set number of averages
    adc->setResolution(16); // set bits of resolution

    // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    adc->enableInterrupts(ADC_0);


    Serial.println("Starting Timers");

    // start the timers, if it's not possible, startTimerValuex will be false
    startTimerValue0 = timer0.begin(timer0_callback, period0);
    // wait enough time for the first timer conversion to finish (depends on resolution and averaging),
    // with 16 averages, 12 bits, and ADC_MED_SPEED in both sampling and conversion speeds it takes about 36 us.
    delayMicroseconds(50); // if we wait less than 36us the timer1 will interrupt the conversion
    // initiated by timer0. The adc_isr will restart the timer0's measurement.

    // You can check with an oscilloscope:
    // Pin 14 corresponds to the timer0 initiating a measurement
    // Pin 15 the same for the timer1
    // Pin 16 is the adc_isr when there's a new measurement on readpin0
    // Pin 17 is the adc_isr when there's a new measurement on readpin1

    // Timer0 starts a comversion and 25 us later timer1 starts a new one, "pausing" the first, about 36 us later timer1's conversion
    // is done, and timer0's is restarted, 36 us later timer0's conversion finishes. About 14 us later timer0 starts a new conversion again.
    // (times don't add up to 120 us because the timer_callbacks and adc_isr take time to execute, about 2.5 us and 1 us, respectively)
    // so in the worst case timer0 gets a new value in about twice as long as it would take alone.
    // if you change the periods, make sure you don't go into a loop, with the timers always interrupting each other

    adc->enableInterrupts(ADC_0);

    Serial.println("Timers started");

    delay(500);
}

int value = 0;
char c=0;

void loop() {

    if(startTimerValue0==false) {
            Serial.println("Timer0 setup failed");
    }

    if(!buffer0->isEmpty()) { // read the values in the buffer
        Serial.println(buffer0->read());
        //Serial.println("New value!");
    }


    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );

    delayMicroseconds(readPeriod);
}
