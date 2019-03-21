#include "Arduino.h"

elapsedMicros time_data;

void setup()
{
  // initialize serial comms
  Serial.begin(115200);
}

void loop()
{
  // read A0
  int val1 = analogRead(6);
  //int temp_time = time_data;
  //time_data = 0;

  // print to serial
  Serial.print(val1);
  //Serial.print(",");
  //Serial.print(temp_time);
  Serial.print("\n");

  // wait
  delay(1);
}
