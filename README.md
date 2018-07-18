# CognionicsQuick20-eeg-plot
Code that allows efficient reading, plotting and impedance check of EEG brain signals from a Cognionics Quick20 EEG headset with 4Hz refreshes. Coded in C and MATLAB. The code pipeline is as follows:
- A session is started and the user is asked to input its code name and run name. A directory based on these values is created.
- The FTDI-chip device is open and a MATLAB engine is initialized
- The software allows a user for a real-time impedance check. If the user chooses so, a MATLAB GUI showing all the channel impedances in real-time is presented. The last measured impedance values of all channels are saved on a CSV file.
- When the run starts, the code reads incoming data from the FTDI chip, transforms it into EEG and accelerometer data and saves it on a CSV file. Periodically (4Hz), it plots the data (across time, frequency and time and frequency - spectrogram) in a MATLAB GUI.
- To finish the app after the plotting starts, the user just needs to press any arbitrary key.

Folder description:
- Dev_Comm: Contains the C code to read/transform/store the FTDI chip data and run the MATLAB engine
- Activity_Plot: Contains the initialization and main loop scripts for plotting the EEG activity.
- Impedance_Real_Time: Contains the script for the impedance check.
