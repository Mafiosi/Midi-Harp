##########################################
##########      LIBRARIES      ###########
##########################################

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


##########################################
##########   CONFIGURATIONS    ###########
##########################################


# Global Variables
global plot_data
global plot_time
global inputs
global figure_capture
global flag_continuously
global flag_plot
global exit_now

# Global Variables Declaration
plot_data = []
plot_time = []

# Plot General Configs
plt.ion()
matplotlib.use("TkAgg")
#matplotlib.use('GTKAgg')


##########################################
#########    PLOT FUNCTIONS    ###########
##########################################

# Data Plot Graph
def plot_figure():

    # Declaration
    global plot_data
    global avg_point
    global high_volt
    global flag_continuously
    global inputs

    # Plot Variables
    plt.ylim(-0.1, high_volt*1.1)
    plt.title("Analogue Piezo Input - Avg Point: " + str(round(avg_point/1000000, 4)) + " ms")
    plt.grid(True)
    plt.ylabel('Voltage Level (V)')
    plt.ticklabel_format(useOffset=False)

    # Plot Print A0
    if inputs > 0:
        plt.plot(plot_data[0], c='b', label='A2', linewidth=1.5)
        plt.legend(loc='upper right')

    # Plot Print A0
    if inputs > 1:
        plt.plot(plot_data[1], c='r', label='A3', linewidth=1.5)
        plt.legend(loc='upper right')

    # Plot Print A0
    if inputs > 2:
        plt.plot(plot_data[2], c='g', label='A3', linewidth=1.5)
        plt.legend(loc='upper right')

    # Plot Print A0
    if inputs > 3:
        plt.plot(plot_data[3], c='y', label='A4', linewidth=1.5)
        plt.legend(loc='upper right')


# Thread for Plotting Graphs
def plotter():

    # Create Data Plot Size
    global plot_data
    global figure_capture
    global flag_continuously
    global exit_now
    global reset

    # Plot Configuration
    style.use('seaborn-notebook')

    # Plotter Cycle
    while True:

        drawnow(plot_figure)
        plt.pause(.000001)

        # Capture Scenario
        if figure_capture is True:
            print("Capturing Image, Close Image to Continue")
            drawnow(plot_figure)
            plt.ioff()
            plt.show()  # Code will Block Here Until Closed Image
            plt.ion()
            figure_capture = False

        if exit_now is True:
            print("Plotter is CLosing")
            return

        if reset:
            while reset:
                pass


##########################################
#########  SERIAL FUNCTIONS    ###########
##########################################


def serial_com(com, baud, n_inputs, window_ref, grid_ref, maxamp_ref, decay_ref, flag_continuously, sample_ref):

    # Establish Serial Communication
    global plot_data
    global plot_time
    global high_volt
    global exit_now
    global avg_point
    global reset

    try:
        serial_main = serial
        ser = serial_main.Serial(com, baud, timeout=1)
    except:
        print("No Devices Connected To\nExiting serial_com Thread")
        return

    # Create Data Plot Size
    for i in range(n_inputs):
        plot_data.append([])
    for i in range(n_inputs):
        plot_time.append([])

    # Counters and Misc
    window = 0
    decay = 0
    avg_point = 0
    maxamp_ref = maxamp_ref*1023/high_volt
    sample = []
    time_base = time.perf_counter_ns()
    for i in range(n_inputs):
        sample.append(0)
    flag_get = False

    # Communication Loop
    while True:

        # Get Serial Line
        try:
            # Collect Serial Data
            serial_data = ser.readline().decode('ascii')

            # Split and Send Serial Data
            split_data = serial_data.split(',')
            for i in range(n_inputs):
                try:
                    # If samples are below threshold point
                    if not flag_continuously:
                        if window <= window_ref and int(split_data[i]) < maxamp_ref:
                            plot_data[i].append(int(split_data[i])*high_volt/1023)
                            plot_time[i].append(time.perf_counter_ns()-time_base)
                            window = window + 1

                        # If more than enough samples have been collected
                        elif window > window_ref and int(split_data[i]) < maxamp_ref:

                            # Check if samples are being collected
                            if flag_get is True:
                                plot_data[i].append(int(split_data[i])*high_volt/1023)
                                plot_time[i].append(time.perf_counter_ns()-time_base)
                                decay = decay + 1

                                # Check about decay after collecting sample
                                if decay > decay_ref:
                                    decay = 0
                                    window = 0
                                    flag_get = False
                                    avg_point = plot_time[0][-1]/decay_ref
                            else:
                                pass

                        # Check if data is useful
                        elif int(split_data[i]) > maxamp_ref:
                            # Updates Time Base
                            if flag_get is False:
                                window = window_ref + 1
                                time_base = time.perf_counter_ns()
                            flag_get = True

                            # Append New data above threshold
                            plot_data[i].append(int(split_data[i])*high_volt/1023)
                            plot_time[i].append(time.perf_counter_ns()-time_base)

                    # Continuous Mode
                    else:
                        if sample[i] == 0:
                            plot_data[i].append(int(split_data[i])*high_volt/1023)
                            plot_time[i].append(time.perf_counter_ns()-time_base)
                            avg_point = (plot_time[0][-1] - plot_time[0][0])/len(plot_time[i])
                            sample[i] = sample[i] + 1
                        elif sample[i] == sample_ref:
                            plot_data[i].append(int(split_data[i])*high_volt/1023)
                            plot_time[i].append(time.perf_counter_ns()-time_base)
                            avg_point = (plot_time[0][-1] - plot_time[0][0])/len(plot_time[i])
                            sample[i] = 0
                        elif sample[i] < sample_ref:
                            sample[i] = sample[i] + 1
                        else:
                            print("Error in Serial, Should Not Happen")
                            sample[i] = 0

                except:
                    pass

        except serial_main.SerialTimeoutException:
            print("Failed to Get Results")
            time.sleep(1)

        # Scale to Established Grid
        for i in range(n_inputs):
            if len(plot_data[i]) > grid_ref:
                plot_data[i].pop(0)
                plot_time[i].pop(0)

        if exit_now is True:
            print("Serial Connection Closing")
            return

        if reset:
            window = 0
            decay = 0
            sample = []
            for i in range(n_inputs):
                sample.append(0)
            flag_get = False
            while reset:
                pass


##########################################
##########    MAIN FUNCTION    ###########
##########################################

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

    ##########################################
    ##########     CONTROL ROOM    ###########
    ##########################################

    com = 'COM3'        # Arduino Communication Port
    baudrate = 2000000  # Arduino Baudrate Communication
    inputs = 1          # How Many Arduino Inputs Reading

    # Control Variables
    window_ref = 50                 # How Many Samples Before Useful Data
    grid_ref = 30000                # How Many Samples will the grid Contain
    maxamp_ref = 0.12                  # From what value is the Data useful (Volts)
    decay_ref = 5000                # How many samples after the data goes below threshold
    high_volt = 3.3                 # What is the maximum voltage accepted by the microcontroller (Volts)
    flag_continuously = False     # Continuous Graph or Sample Data above threshold
    sample_ref = 2                  # CONTINUOUS ONLY: How many samples should be skipped (prevent lag)

    ##########################################
    ##########   EOF CONTROL ROOM  ###########
    ##########################################

    # Counters and Misc
    figure_capture = False
    exit_now = False
    reset = False

    # Check for Correct Behaviour
    try:
        serial.Serial(com, baudrate, timeout=1)
    except:
        print("No Devices Connected\nExiting Program")
        return

    # Thread Definition
    thread_serial = threading.Thread(name='Serial_Communication', target=serial_com, args=(com, baudrate, inputs,
                                        window_ref, grid_ref, maxamp_ref, decay_ref, flag_continuously, sample_ref,))
    thread_plotter = threading.Thread(name='Serial_Plotter', target=plotter)

    ## Thread Start
    thread_serial.start()
    thread_plotter.start()

    # Accept User Input
    user_input = input("Tip: In case of doubt use the 'help' command\nINSERT YOUR COMMAND:")
    while True:

        # Analysis on Captured Figure
        if user_input == 'stop':
            figure_capture = True
            while figure_capture is True:
                time.sleep(1)

        # Exit Command
        elif user_input == 'exit':
            exit_now = True
            break

        # Reset List
        elif user_input == 'reset':
            reset = True
            time.sleep(1)
            plot_data = []
            plot_time = []
            for i in range(inputs):
                plot_data.append([])
                plot_time.append([])

            reset = False

        # Help Command
        elif user_input == 'help':
            print("\n" +
                  "-->     stop  - Capture Image to Analyse\n" +
                  "-->     exit  - Exit Program Safely (Serial)\n" +
                  "-->     help  - Displays this Menu\n" +
                  "-->     reset - Resets Graphical Data\n" +
                  "")

        else:
            print("Command not Found, Type 'help' to display help Menu\n")

        user_input = input("INSERT YOUR COMMAND: ")

    # Waits for Work to Finish
    thread_serial.join()
    thread_plotter.join()
    time.sleep(2)


if __name__ == '__main__':
    main()

