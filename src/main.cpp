
#include <Arduino.h>

/////////////////////////////////////////////////
/////////    SERIAL COMMUNICATION       /////////
/////////////////////////////////////////////////

// SERIAL Receiver Interrupt


// SETUP PROCESS
void setup()
{
  // MONITORING VARIABLES
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

  // END OF CYCLE ROUTINE
  delay(2);
}
