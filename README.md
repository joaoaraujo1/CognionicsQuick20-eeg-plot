# Cognionics Quick20 Projects

## Prototype of a working closed-loop BCI (DYS_BCI)
First iteration of a dyslexia BCI using auditory input and visual feedback. Based on the BCI Award 2022 nominee project [book chapter](https://link.springer.com/chapter/10.1007/978-3-031-49457-4_2) and on the modeling work provided on [this paper](https://www.frontiersin.org/journals/human-neuroscience/articles/10.3389/fnhum.2024.1403677/full).
Written in Python using the ftd2xx library for chip reading and pygame for the paradigm. Includes a simple algorithm for online detection of blinking artifacts and signal correction with baselne distribution sampling. Schematics below:

.

.

![image](https://raw.githubusercontent.com/joaoaraujo1/CognionicsQuick20-eeg-plot/master/DYS_BCI/BCI_schematic.png)

.


## CognionicsQuick20-eeg-plot (Dev_Comm, Activity_Plot, Impedance_Real_Time)
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


Screenshots:

EEG pattern of a subject with his eyes open:
![eyesopen](https://user-images.githubusercontent.com/40466329/46423755-33b9b900-c72f-11e8-94d2-b752a0e1f099.jpg)

EEG pattern of a subject with his eyes closed:
![eyesclosed](https://user-images.githubusercontent.com/40466329/46423732-23094300-c72f-11e8-8802-f33c589ab390.jpg)

Electrode impedance check:
<img width="1280" alt="real time impedance ss" src="https://user-images.githubusercontent.com/40466329/46423810-59df5900-c72f-11e8-9087-51919de8365b.png">
