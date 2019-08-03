# Adaptation of an Harp for MIDI Implementation

A stable MIDI Interface to be applied on an Harp to convert a string vibration
using piezoelectronics into a MIDI message.


## Used Devices

This Implementation should work using either:

- [Arduino UNO](https://store.arduino.cc/arduino-uno-rev3)
- [Teensy 3.5](https://www.pjrc.com/store/teensy35.html)

The Teensy 3.5 is recommended due to higher analog input availability.


## Repository Contents

This Repository Contains:

- Internal Fast Fourier Transform - Sends Processed data for graphical visualization (C++)
- External Fast Fourier Transform - Receives data to process and visualize (Python)
- External Logger - Allows visualization in real-time of several analog inputs (Python)
- ADC(s) Validation - Allows finer tuning of internal ADC configurations regarding timing windows (C++)
- MIDI Converter - MAIN PROJECT - Converts string vibration of an harp into a MIDI message (C++)

All of these programs are cointained within this repository.  
There are also several results examples from the graphical Interface as well the ADC Validation within
the folder "Graphical Result".

## Author
**João Beleza**
