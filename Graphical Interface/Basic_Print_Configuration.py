# Communication
import serial
# Generic
import time
import threading
from queue import Queue
import datetime

# Communication
import serial

# Graph Generation
import matplotlib.pyplot as plt
import matplotlib.animation as anime
from matplotlib import style
import matplotlib
from drawnow import *
import numpy as np


def main():

    # Global Variables
    global inputs
    global figure_capture
    global exit_now
    global plot_data
    global plot_time
    global high_volt
    global flag_continuously
    global reset
    global bitresolution

    ##########################################
    ##########     CONTROL ROOM    ###########
    ##########################################

    com = 'COM3'        # Arduino Communication Port
    baudrate = 115200  # Arduino Baudrate Communication
    inputs = 1          # How Many Arduino Inputs Reading

    # Control Variables
    window_ref = 50                 # How Many Samples Before Useful Data
    grid_ref = 20000            # How Many Samples will the grid Contain
    maxamp_ref = 0.12                  # From what value is the Data useful (Volts)
    decay_ref = 5000                # How many samples after the data goes below threshold
    high_volt = 3.3                 # What is the maximum voltage accepted by the microcontroller (Volts)
    flag_continuously = True     # Continuous Graph or Sample Data above threshold
    sample_ref = 1                  # CONTINUOUS ONLY: How many samples should be skipped (prevent lag)
    bitresolution = 65536

    ##########################################
    ##########   EOF CONTROL ROOM  ###########
    ##########################################

    # Counters and Misc
    figure_capture = False
    exit_now = False
    reset = False

    # Check for Correct Behaviour
    try:
        serial_main = serial
        ser = serial_main.Serial(com, baudrate, timeout=1)
    except:
        print("No Devices Connected\nExiting Program")
        return

    while True:
        try:
            serial_data = ser.readline().decode('ascii')
            seri = serial_data.rstrip()
            print(seri)
            if(serial_data > 65535):
                print("here")
        except:
            pass


if __name__ == '__main__':
    main()