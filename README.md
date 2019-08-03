# Adaptation of an Harp for MIDI Implementation

A stable MIDI Interface to be applied on an Harp to convert a string vibration using piezoelectronics into a MIDI message


## Used Devices

This Implementation should work using either:

- [Arduino UNO Rev.3](https://store.arduino.cc/arduino-uno-rev3)
- [Teensy 3.5](https://www.pjrc.com/store/teensy35.html)

The Teensy 3.5 is recommended due to higher analog input availability


## Repository Contents

This Repository Contains:

- Internal Fast Fourier Transform - Sends Processed data for graphical visualization (C++)
- External Fast Fourier Transform - Receives data to process and visualize (Python)
- External Logger - Allows visualization in real-time of several analog inputs (Python)
- ADC(s) Validation - Allows finer tuning of internal ADC configurations regarding timing windows (C++)
- MIDI Converter - MAIN PROJECT - Converts string vibration of an harp into a MIDI message (C++)

All of these programs are cointained within this repository.  There are also several results examples from the graphical Interface as well the ADC Validation within the folder "Graphical Result".

## Features

The Main Program - MIDI Converter - allows both to send data as a MIDI message as well as a raw analog signal for visualization using the python logger - External Logger  

This program can perform for both Celtic and Pedal Harps (using the Teensy 3.5) with an average delay of 1ms using averaging and redundance techniques  

It is also possible to send the MIDI message by performing an analysis in the frequency domain using the Fast Fourier Transform for an auto-tuning implementation. The trade-off in this implementation is an average delay of 5-8ms  (NOT RECOMMENDED AS IT IS)  

The Graphical Visualizers for both raw analog signal output and Fast Fourier Transform result allow control full over the data window and data collection in real-time  

### Author
**João Beleza**