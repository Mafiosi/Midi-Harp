/////////////////////////////////////////////////
/////////           LIBRARIES           /////////
/////////////////////////////////////////////////

#include <Arduino.h>

/////////////////////////////////////////////////
/////////    SERIAL COMMUNICATION       /////////
/////////////////////////////////////////////////

// SETUP PROCESS
void setup()
{
  // MONITORING VARIABLES
  Serial.begin(115200);
}

/////////////////////////////////////////////////
/////////     TUNABLE CONFIGURATION     /////////
/////////////////////////////////////////////////

// MIDI CONTROL VARIABLES
const byte MIDI_CHANNEL = 1;    // MIDI CHANNEL NUMBER
const byte MIDI_n_inputs = 1;   // HOW MANY INPUTS WILL BE READ

// GENERIC CONTROL VARIABLES
const bool use_general_timer = 0;             // TIMER BETWEEN ADC USE
const bool Serial_print = 1;                  // PRINT MESSAGES TO SERIAL
const bool debug_print = 0;                   // Print Debug Messages
const byte input_pins[MIDI_n_inputs] = {6};   // ANALOGUE INPUT PINS
const byte input_pitch[MIDI_n_inputs] = {60}; // PITCH ASSOCIATED WITH EACH INPUT

/////////////////////////////////////////////////
/////////    PERMANENT  CONFIGURATION   /////////
/////////////////////////////////////////////////

// TIMER CONTROL VARTIABLES
byte unsigned general_timer_value = 250;   // HOW MUCH TIME BETWEEN READING
int unsigned single_timer_value = 350;    // HOW MUCH TIME BETWEEN READING SAME INPUT

//MIDI CONTROL VARIABLES
const byte unsigned threshold_MIN = 38;     // MINIMUM VALUE TO ACCEPT AS MESSAGE (0.1 Volt)
const int unsigned threshold_MAX = 775;     // MAXIMUM VALUE EQUAL TO MAX MIDI VELOCITY (2.5 Volt)
const byte unsigned threshold_MIN_OFF = 16; // MINIMUM VALUE TO SEND MIDI NOTE OFF (0.05 Volt)
const int unsigned threshold_delay = 1000;  // MINIMUM VALUE TO WAIT AFTER SENDING MIDI OFF

// SAMPLE CONTROL
const byte unsigned sample_n_read = 10;  // HOW MANY SAMPLES TO COLLECT MAX

// SERIAL PRINT CONTROL
const int unsigned threshold_print_MAX = 1023;
bool print_active = 0;
const byte fake_value_MIDI_print = 5;

/////////////////////////////////////////////////
/////////     VARIABLES DECLARATION     /////////
/////////////////////////////////////////////////

// ANALOGUE INPUT CONTROL
int unsigned input_read[MIDI_n_inputs];
int unsigned input_n_sample_read[MIDI_n_inputs];
byte unsigned input_state[MIDI_n_inputs];

// TIMER CONTROL VARIABLES
elapsedMicros general_timer = general_timer_value;
elapsedMicros single_timer[MIDI_n_inputs];

// MIDI CONTROL VARIABLES
bool played[MIDI_n_inputs];

// SERIAL PRINT CONTROL VARIABLES
int unsigned prints[MIDI_n_inputs*2];

/////////////////////////////////////////////////
/////////     FUNCTIONS DECLARATION     /////////
/////////////////////////////////////////////////

void MIDI_sendON(byte pitch, byte velocity){
  usbMIDI.sendNoteOn(pitch, velocity, MIDI_CHANNEL);
}

void MIDI_sendOFF(byte pitch){
  usbMIDI.sendNoteOff(pitch, 0, MIDI_CHANNEL);
}

byte normalizevelocity(int input_read){
  byte velocity = input_read*127/threshold_MAX;
  return velocity;
}


/////////////////////////////////////////////////
/////////           MAIN LOOP           /////////
/////////////////////////////////////////////////

void loop(){

  // INITIAL SETUP
  for(byte unsigned temp = 0; temp < MIDI_n_inputs; temp++){
    input_read[temp] = 0;
    input_n_sample_read[temp] = 0;
    input_state[temp] = 0;
    single_timer[temp] = 0;
    played[temp] = 0;
    prints[temp*2] = threshold_print_MAX;
    prints[temp*2+1] = threshold_print_MAX;
  }

  // MAIN LOOP
  while(1){

    // READ EACH INDIVIDUAL PIN
    for(byte unsigned pin = 0; pin < MIDI_n_inputs; pin++){

      // WAIT BEFORE USING ADC TOO PREVENT ERROR (IF ENABLED)
      if(use_general_timer == 1){
        while(general_timer <= general_timer_value){
          // DO NOTHING BUT WAIT
        }
      }

      // STATE 0 - COLLECT ANALOGUE AND READ RECEIVED SIGNAL
      if(input_state[pin] == 0){

        if(single_timer[pin] >= single_timer_value){

          // Get Sample
          input_read[pin] = analogRead(input_pins[pin]);
          single_timer[pin] = 0;

          // Reset General Timer if Enabled
          if(use_general_timer == 1){
            general_timer = 0;
          }

          // Print If Enabled
          if(Serial_print == 1){
            prints[pin*2] = input_read[pin];
            print_active = 1;
          }

          // Check if conditions are met to collect sample
          if(input_read[pin] >= threshold_MIN){
            input_state[pin] = 1;
          }

          // Debug Print
          else if(debug_print && print_active){
            Serial.print("[PIN] - ");
            Serial.print(pin);
            Serial.print(" STATE 0 - ");
          }
        }
      }

      // STATE 1 -  SEND MIDI ON AND WAIT TO SEND OFF
      if(input_state[pin] == 1){

        // IF Message has not been Sent
        if(played[pin] == 0){

          // Send Midi On Message
          MIDI_sendON(input_pitch[pin], normalizevelocity(input_read[pin]));
          played[pin] = 1;
          input_n_sample_read[pin] = 1;

          // If Print Enable Send Midi Print
          if(Serial_print == 1){
            prints[pin*2 + 1] = input_read[pin];
            print_active = 1;
          }

          // Debug Print
          if(debug_print){
            Serial.print("[PIN] - ");
            Serial.print(pin);
            Serial.print(" STATE 1 - ");
          }

          // Reset input_read
          input_read[pin] = 0;
        }

        // IF Message has already been Sent
        else{
          if(single_timer[pin] >= single_timer_value){

            // Read Input To Check Value and Increment Counter
            int unsigned temp = analogRead(input_pins[pin]);
            input_n_sample_read[pin] = input_n_sample_read[pin] + 1;
            single_timer[pin] = 0;

            // Reset General Timer if Enabled
            if(use_general_timer == 1){
              general_timer = 0;
            }

            // Collect Highest Value
            if(temp > input_read[pin]){
              input_read[pin] = temp;
            }

            // Check Counter
            if(input_n_sample_read[pin] >= sample_n_read){
              if(input_read[pin] <= threshold_MIN_OFF){
                input_state[pin] = 2;
              }

              // Print If Enabled (Midi Value)
              if(Serial_print == 1){
                prints[pin*2 + 1] = input_read[pin];
              }

              // Reset Variables
              input_n_sample_read[pin] = 0;
              input_read[pin] = 0;
            }

            // Print Graph Value
            if(Serial_print == 1){
              prints[pin*2] = temp;
              print_active = 1;
            }

            // Debug Print
            if(debug_print && input_state[pin] == 1){
              Serial.print("[PIN] - ");
              Serial.print(pin);
              Serial.print(" STATE 1 - ");
            }
          }
        }
      }

      // STATE 2 - SEND MIDI OFF AND RESET
      if(input_state[pin] == 2){

        if(played[pin] == 1){
          MIDI_sendOFF(input_pitch[pin]);

          //RESET
          input_state[pin] = 0;
          played[pin] = 0;
          single_timer[pin] = 0;

          // Debug Print
          if(debug_print){
            Serial.print("[PIN] - ");
            Serial.print(pin);
            Serial.print(" STATE 2 - ");
          }
        }

        // Wait Tim to Set State to Zero
        else{
          if(single_timer[pin] >= threshold_delay){
            input_state[pin] = 0;
          }

          // Print If Enabled
          else if(Serial_print == 1){
            input_read[pin] = analogRead(input_pins[pin]);
            prints[pin*2] = input_read[pin];
            print_active = 1;
          }



        }
      }
    }

    // PRINT IF NEEDED
    if((Serial_print == 1) && (print_active == 1)){
      for(byte unsigned temp = 0; temp < MIDI_n_inputs*2; temp++){

        // IF value is not worth it
        if(prints[temp] == threshold_print_MAX){
          Serial.print(fake_value_MIDI_print);
          Serial.print(",");
        }
        else{
          Serial.print(prints[temp]);
          Serial.print(",");
        }

        // Reset prints variables
        prints[temp] = threshold_print_MAX;
      }

      Serial.print("\n");
      // Reset Print Flag
      print_active = 0;
    }
  }
}
