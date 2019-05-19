
#include <Arduino.h>
#include <Bounce.h>

// the MIDI channel number to send messages
const int channel = 1;

// the MIDI continuous controller for each analog input
const int controllerA0 = 10; // 10 = pan position
const int controllerA1 = 11; // 11 = volume/expression
const int controllerA2 = 91; // 91 = reverb level
const int controllerA3 = 93; // 93 = chorus level

void setup() {
  Serial.begin(115200);
}

// store previously sent values, to detect changes
int previousA0 = -1;
int previousA1 = -1;
int previousA2 = -1;
int previousA3 = -1;

elapsedMillis msec = 0;


void loop() {
  // only check the analog inputs 50 times per second,
  // to prevent a flood of MIDI messages
  if (msec >= 20) {
    msec = 0;
    int n0 = analogRead(A6) / 8;


    // only transmit MIDI messages if analog input changed
    if (n0 != previousA0) {
      usbMIDI.sendNoteOn(n0, 100, channel);
      previousA0 = n0;
      Serial.println(n0);
    }
  }

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}
