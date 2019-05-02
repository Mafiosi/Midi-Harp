#include "Arduino.h"

elapsedMicros time_data;

void setup()
{
  // initialize serial comms
  Serial.begin(115200);
}

elapsedMicros wait = 0;
int unsigned wait_time = 250;
byte pin = 0;
int val1 = 0;
int val2 = 0;

void loop()
{
  // read A0
  if(pin == 0 && wait >= wait_time){

    wait = 0;
    val1 = analogRead(9);
    pin = 1;
  }

  // read A0
  if(pin == 1 && wait >= wait_time){

    wait = 0;
    val2 = analogRead(12);
    pin = 2;
  }

  if(pin == 2){
    // print to serial
    Serial.print(val1);
    Serial.print(",");
    Serial.print(val2);
    Serial.print("\n");
    pin = 0;
  }
}
