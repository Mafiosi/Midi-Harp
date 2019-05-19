##########################################
##########      LIBRARIES      ###########
##########################################

# Generic
import time

# Communication
import serial

# Graph Generation
import matplotlib.pyplot as plt
import matplotlib.animation as anime
from matplotlib import style
import matplotlib
from drawnow import *
import numpy as np
from numpy import *
from matplotlib.pyplot import *


plt.ion()
matplotlib.use("TkAgg")
rcParams['mathtext.fontset'] = 'custom'
rcParams['mathtext.it'] = 'STIXGeneral:italic'
rcParams['mathtext.bf'] = 'STIXGeneral:italic:bold'

##########################################
##########    MAIN FUNCTION    ###########
##########################################

def main():


    ##########################################
    ##########     CONTROL ROOM    ###########
    ##########################################

    com = 'COM3'            # Arduino Communication Port
    baudrate = 2000000      # Arduino Baudrate Communication
    sample_size = 1024
    sample_freq = 50000
    flag_db = False


    ##########################################
    ##########   EOF CONTROL ROOM  ###########
    ##########################################

    plot_data = []
    plot_freq = []
    plot_freq_true = []
    fine_plot_data = []
    flag_get = False
    true_sample_size = int(sample_size/2)
    true_sample_freq = int(sample_freq/2)
    interval = true_sample_freq / true_sample_size

    for i in range(true_sample_size):
        plot_freq.append(interval*i+interval/2)
        plot_freq_true.append(interval*i)

    plot_freq_true.append(true_sample_freq)
    # Check for Correct Behaviour
    try:
        serial_main = serial
        ser = serial_main.Serial(com, baudrate, timeout=1)
    except:
        print("No Devices Connected\nExiting Program")
        return

    # Communication Loop
    print("Waiting for Input")
    while True:

        # Get Serial Line
        try:
            # Collect Serial Data
            serial_data = ser.readline().decode('ascii')

            # Split and Send Serial Data
            split_data = serial_data.split(',')
            try:
                if len(plot_data) < true_sample_size:
                    plot_data.append((int(split_data[0])))

                    if flag_get is False:
                        print("Starting FFT Process")
                        flag_get = True

                else:
                    print("Finished FFT Process")
                    break
            except:
                pass

        except serial_main.SerialTimeoutException:
            print("Failed to Get Results")
            time.sleep(1)

    reference = plot_data[0]
    fine_plot_data.append(0)
    for i in range(len(plot_data)-1):
        fine_plot_data.append(20*np.log10(plot_data[i+1]/reference))

    y_func = np.linspace(0, true_sample_freq, int(interval))
    y_funk = np.arange(0, 5000, 48)

    if flag_db is True:
        plt.plot(plot_freq, fine_plot_data, label='FFT - F4 (349.43 Hz)', linewidth=1)
        plt.ylim(-100, 10)
        plt.ylabel('Fast Fourier Transform -' + r'$db$')
    else:
        plt.plot(plot_freq, plot_data, label='FFT - F4 (349.43' + r'$Hz$' + ')',linewidth=1)
        plt.ylim(-0.1, 6500)
        plt.ylabel('Fast Fourier Transform (db/dec)')
        plt.ylabel('Fast Fourier Transform - ' + r'$V^2/Hz$')
    plt.xticks(plot_freq_true, rotation = '65', horizontalalignment = 'center')
    plt.xlim(-0.1, 5000)
    plt.xlabel('Frequency Bins - ' + r'$Hz$')
    plt.grid(True)
    plt.legend(loc='upper right')
    plt.title('Fast-Fourier Transform')
    plt.show()
    plt.pause(.000001)

    plt.ioff()
    plt.show()

    while True:
        # DO NOTHING
        pass




if __name__ == '__main__':
    main()