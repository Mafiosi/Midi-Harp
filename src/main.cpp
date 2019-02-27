/*
#include <Arduino.h>
#include "Plotter.h"

/////////////////////////////////////////////////
/////////    SERIAL COMMUNICATION       /////////
/////////////////////////////////////////////////

// SERIAL Receiver Interrupt


// SETUP PROCESS
void setup()
{
  // MONITORING VARIABLES
  Serial.begin(115200);
  //Serial.onReceive(serial_receiver)

  //p.Begin();

  //PLOTTING GRAPH
  //p.AddTimeGraph("Piezo_Input_A0", 10000, "Piezo",read_val_A2);

}




void loop()
{
  int read_val_A2 = analogRead(2);
  //Serial.print("analog 0 is: ");
  //Serial.print(read_val_A2);
  //Serial.println();
  //p.Plot();
  Serial.print(read_val_A2);
  Serial.print(",");
  Serial.print(0);
  Serial.println();

  // END OF CYCLE ROUTINE
  delay(2);

  // Check user Input
  if(Serial.available() > 0) {
    Serial.println("Got Input");
  }
  //delay(1);
}
*/
// analog-plot
//
// Read analog values from A0 and A1 and print them to serial port.
//
// electronut.in

#include "Arduino.h"

void setup()
{
  // initialize serial comms
  Serial.begin(2000000);
}

void loop()
{
  // read A0
  int val1 = analogRead(2);
  // read A1
  int val2 = analogRead(4);
  // print to serial
  Serial.print(val1);
  Serial.print(",");
  Serial.print(val2);
  Serial.print("\n");
  // wait
  delay(10);
}
