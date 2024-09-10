import ftd2xx as ftd
import time
import numpy as np
import matplotlib.pyplot as plt
import datetime
import pygame
import wave
import pandas as pd

from pathlib import Path
from scipy.signal import welch, butter, sosfiltfilt

import sys
import joblib
from collections import deque

import warnings
warnings.filterwarnings("ignore")

### CGX FUNCTIONS
def CGX_start():

    """
    Device Open
    """
    d = ftd.open(0)    
    
    """
    Device Set
    """
    # Get FTDI device Info
    #ftdi_data_ = d.eeRead()
    #device_description = ftdi_data_.Description
    #device_serial = ftdi_data_.SerialNumber
    #device_info = d.getDeviceInfo()
    
    # Set flow control
    d.setFlowControl(ftd.defines.FLOW_RTS_CTS, 17, 19)
    
    # Set Baud Rate
    d.setBaudRate(CGX_BAUD_RATE)
    
    # Set data characteristics using the programmers manual
    d.setDataCharacteristics(ftd.defines.BITS_8, ftd.defines.STOP_BITS_1, ftd.defines.PARITY_NONE)
    
    # Set latency
    d.setLatencyTimer(2)
    
    # Purge device from "old" data
    rx_tx_events_tuple = (-1,-1)
    while rx_tx_events_tuple[0] != 0 or rx_tx_events_tuple[1] != 0:
        d.purge(ftd.defines.PURGE_RX | ftd.defines.PURGE_TX)
        rx_tx_events_tuple = d.getStatus() # check if purge was successful
        print(rx_tx_events_tuple)
    
    # Disable impedance check
    d.write(bytes([0x12]))
    
    return d

def CGX_read(d,previous_packet):
    # Packet counter and packet loss warning
    packet_byte = ord(d.read(1))
    
    if previous_packet > 127:
        previous_packet = packet_byte
    else:
        if packet_byte != previous_packet + 1 and packet_byte != 0 and not start_timer:
            print('WARNING! PACKETS LOST!')
        previous_packet = packet_byte
    
    # EEG and accelerometer data
    eeg = np.zeros((N_CHANS,1))
    for i in range(N_CHANS):
        buf = d.read(3)
        eeg_dec = (buf[0] << 24) | (buf[1] << 17) | (buf[2] << 10)
        eeg[i] = eeg_dec * (5.0 / 3.0) * (1.0 / pow(2, 32))  # in Volts
    
    acc = np.zeros((N_ACC,1))
    for i in range(N_ACC):
        buf = d.read(3)
        acc_dec = (buf[0] << 24) | (buf[1] << 17) | (buf[2] << 10)
        acc_v = acc_dec * 2.5 * ( 1.0 / pow(2, 32) )  # in Volts
        acc[i] = pow(2,18) * (acc_v / (pow(10,6) * 2.5 - 512) ) * (1/143.3)
    
    # Impedance check
    impedance_check = ord(d.read(1)) # 0x11 (17) ON, 0x12 (18) OFF
    
    # Battery status
    battery_byte = d.read(1)
    battery_status = ord(battery_byte) * 5.0/128.0
    
    # Trigger data
    trigger_bytes = d.read(2)
    trigger_dec = (trigger_bytes[0] << 8) + trigger_bytes[1]
    
    # Read unknown sequence of 3 bytes >>>> FOR NEW SYSTEMS ONLY
    d.read(3)
    
    return (packet_byte,eeg,acc,impedance_check,battery_status,trigger_dec)


### DataFrame column lists

ACC_NAMES = [
    'X_ACC',
    'Y_ACC',
    'Z_ACC']
EEG_NAMES = [
    'F7',
    'Fp1',
    'Fp2',
    'F8',
    'F3',
    'Fz',
    'F4',
    'C3',
    'Cz',
    'P8',
    'P7',
    'Pz',
    'P4',
    'T3',
    'P3',
    'O1',
    'O2',
    'C4',
    'T4',
    'A2',
    'ExG1']
HEADERS =  ['timeStamp','eventId','packetId','batteryV','ship_#idx','ITI']

data_columns = HEADERS + EEG_NAMES + ACC_NAMES
df_Data = pd.DataFrame(data=np.zeros((int(6e5),len(data_columns))),columns=data_columns)
df_Data['timeStamp'].astype(str)

### SOUND FUNCTIONS

# Get story file info
def get_wav_info(wav_filename):
    with wave.open(wav_filename, 'rb') as wave_file:
        channels = wave_file.getnchannels()
        frame_rate = wave_file.getframerate()
        frames = wave_file.getnframes()
        duration = frames / float(frame_rate)
    return channels, frame_rate, frames, duration


# Pygame check events
def PG_event_check(playingStory):
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            playingStory = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                playingStory = False
    return playingStory

# Online artifact control
def ThresholdArtifactRemoval(filtered_window,
                            index_data,
                            len_window,
                            ART_THRESH,
                            art_HM,
                            baseline_dist):
    
    # Shift corrections by the length size of the new incoming data
    #print(len_window)
    channels_n = filtered_window.shape[1]
    old_HM = art_HM.copy()
    old_keys = old_HM.keys()
    for window_idx in old_keys:
        if window_idx - len_window >= 0:
            art_HM[window_idx - len_window] = art_HM[window_idx]
            filtered_window[window_idx - len_window,:] = art_HM[window_idx - len_window]
        del art_HM[window_idx]
    
    # If at least one channel datapoint is noisy, we replace the datapoint from
    # the other channels too using similar indices in the baseline data. This allows 
    # the cross correlations between channels to be kept in this process
    new_window = filtered_window[-len_window:,:]
    above_thresh_idcs = []
    for i in range(channels_n):
        above_thresh_idcs_i = np.where(np.abs(new_window[:,i]) > ART_THRESH[i])
        above_thresh_idcs += list(above_thresh_idcs_i[0])
    above_thresh_idcs = np.unique(above_thresh_idcs)
    
    # Replace tainted EEG idcs with baseline values
    if len(above_thresh_idcs) > 0:
        random_baseline_idcs = np.random.randint(0,baseline_dist.shape[0],len(above_thresh_idcs))
        for i in range(len(above_thresh_idcs)):
            filtered_window[filtered_window.shape[0] - len_window + above_thresh_idcs[i]] = baseline_dist[random_baseline_idcs[i],:]
            art_HM[filtered_window.shape[0] - len_window + above_thresh_idcs[i]] = baseline_dist[random_baseline_idcs[i],:]
            ART_HM_FULL[index_data + above_thresh_idcs[i]] = baseline_dist[random_baseline_idcs[i],:]
    
    #print(len(art_HM))
    return filtered_window, art_HM
    


### PROGRAM STARTS HERE ###
subj_name = input("Enter participant's name: ")
session_n = input("Enter session number: ")

path_string = "" + subj_name + "/" + session_n

Path(path_string).mkdir(parents=True, exist_ok=False)

"""
Load story based on session number
Paradigm - each story is heard about 10 times
"""
if   int(session_n) <= 10:
    story_name = 'WinnieThePooh.wav'
elif int(session_n) <= 20:
    story_name = 'Part1.wav'
elif int(session_n) <= 30:
    story_name = 'Part2.wav'
elif int(session_n) <= 40:
    story_name = 'Part3.wav'
else:
    story_name = 'Part4.wav'
_, _, _, story_len_s = get_wav_info(story_name)

### CONSTANTS

CGX_BAUD_RATE = np.uint32(1000000)
N_CHANS = 21
N_ACC = 3
COOLING_PERIOD_SECONDS = 5
SAMPLING_FREQ = 500
REFRESH_RATE_S = .25
PACKET_SIZE = 78
MAX_V = 2.4
ARTIFACT_REMOVAL_CDF_100 = 95 
WINDOW_LEN = 3 * SAMPLING_FREQ               # Window length in samples
    
### MAIN GAME VARS ###
baseline_position_purge = 12                 # number of positions to purge from baseline calc
BASELINE = [True]                            # Global for task state
BASELINE_TIME = 4 * 60                       # baseline time in seconds
POS_HIST = deque([0.5] * 4)                  # GLOBAL VARIABLE for spaceship position latest history
POS_HIST_TOTAL = deque()                     # GLOBAL VARIABLE for spaceship position full history
TIME_WINDOW = np.zeros((WINDOW_LEN,N_CHANS)) # Sliding window for EEG data
central_channels = np.array([EEG_NAMES.index('Cz'),
                             EEG_NAMES.index('C3'),
                             EEG_NAMES.index('C4'),
                             EEG_NAMES.index('F3'),
                             EEG_NAMES.index('F4'),
                             EEG_NAMES.index('P3'),
                             EEG_NAMES.index('P4')])
SUBJ_STATS = [0,1] # median, std
ship_position_idx = 0

# Misc vars
overall_time = np.inf
start_timer = True
start_recording = False
extra_packets = 0
total_packets_lost = 0
seconds_in = 0.0
previous_packet = np.inf
impedance_check = 17 
cooling_timer = time.perf_counter()
data_timer = 0
playingStory = True
scoreScreen = False
index_data = 0
t0 = 0
baseline_dist = []

# Bandpass filter
bp_filter_sos = butter(4, 
                       [0.5,10], 
                       btype='bandpass', 
                       analog=False, 
                       output='sos', 
                       fs=SAMPLING_FREQ)

def GetFilteredWindow(time_window,sos,central_channels):
    output_window = np.zeros((time_window.shape[0],len(central_channels)))
    for i in range(len(central_channels)):
        output_window[:,i] = sosfiltfilt(sos, 
                                         time_window[:,central_channels[i]],
                                         padlen = sos.shape[0]) # padding == filter order (no shift on freq peaks on tests)
    return output_window


# PYGAME variables
story_cursor_pos = 0

pygame.init()
pygame.mixer.init()
pygame.mixer.music.load(story_name)
PG_event_check(playingStory)
font = pygame.font.SysFont('Arial.ttf', 72)
baseline_txt = font.render('Good luck! Please wait...', True, (200,100,100))

screen = pygame.display.set_mode()
w, h = pygame.display.get_surface().get_size()
background = pygame.image.load('background.jpg')
background = pygame.transform.scale(background, (w, h))
s = pygame.Surface((w,h), pygame.SRCALPHA)   # per-pixel alpha
trace_width = int(h/100)                     # width of the trace
trace_color = (100,100,100)                  # position history color
ship_width, ship_height = w/10, h/10         # Ship dimensions
midline_width = int(h/100)                   # midline width
ship = pygame.image.load('spaceship.png')
ship = pygame.transform.scale(ship, (ship_width, ship_height))

smiley_img = pygame.image.load('smiley.jpg')
smiley_img = pygame.transform.scale(smiley_img, (ship_width, ship_width))

progress_bar_width = w/40
score = 0

# Threshold removal vars
art_HM      = dict() # temporary artifact hashmap
ART_HM_FULL = dict() # full artifact hashmap
ART_THRESH = np.zeros(central_channels.shape) # Threshold


######## MAIN #########


# Start CGX
d = CGX_start()

while playingStory:
    
    # Check quit events
    PG_event_check(playingStory)
    
    # Read only if we have bytes in buffer
    # Do not get stuck otherwise
    has_bytes = False
    r_bytes = d.getStatus()[0]
    if r_bytes >= PACKET_SIZE:
        cur_byte = ord(d.read(1))
        has_bytes = True

    if not has_bytes or cur_byte == 255: # Starting packet
        
        if start_timer:
            
            # Wait for cooling period to end and make sure to disable
            # impedance checks for high bandwidth
            if time.perf_counter() - cooling_timer > COOLING_PERIOD_SECONDS:
                if impedance_check == 18: 
                    start_timer = False
                    extra_packets,total_packets_lost = 0,0
                    data_timer = time.perf_counter()
                    t0 = time.perf_counter()
                    data_window = deque()
                else:
                    # Try to disable impedance check  again and reset cooling timer
                    d.write(bytes([0x12]))
                    cooling_timer = time.perf_counter()
                    
        else:
            if extra_packets > 0:
                total_packets_lost += extra_packets
                print(f'WARNING! {extra_packets} bytes read between packets! Total bytes lost: {total_packets_lost}')
                extra_packets = 0
        
        #### System / soundfile vars ####
        # Read and parse data if we have bytes in buffer
        if has_bytes:
            # Read full packet
            (packet_byte,eeg,acc,impedance_check,battery_status,trigger_dec) = CGX_read(d, previous_packet)
            
            # Timestamp
            timestamp = str(datetime.datetime.now())
            
            # Sound events - approximately every 1 second
            event = 0
            if pygame.mixer.get_init():
                story_cursor_pos = pygame.mixer.music.get_pos() / 1000
                if story_cursor_pos > seconds_in:
                    event = story_cursor_pos
                    seconds_in += 1
                    #print(event)
                elif (story_cursor_pos < 0 and not BASELINE[-1]): # story cursor at 0 after playing
                    playingStory = False
                    scoreScreen = True # bring up the score screen


        ### Timer started - baseline or experiment snippet
        if not start_timer:
            # Build data window
            if has_bytes:
                data_entry = [timestamp,event,packet_byte,battery_status,ship_position_idx,0] + list(np.squeeze(eeg)) + list(np.squeeze(acc))        
                data_window.append(data_entry)
            
            iti = time.perf_counter() - data_timer
            if iti > REFRESH_RATE_S and len(data_window) > 0:
                #print(iti)
                data_timer  = time.perf_counter() # update timer ASAP
                
                # Update DataFrame
                len_window = len(data_window)
                new_data = np.array(data_window)
                new_data[:,HEADERS.index('ITI')] = iti
                df_Data.iloc[index_data:index_data+len_window,:] = new_data
                new_data = new_data[:,len(HEADERS):len(HEADERS)+N_CHANS].astype(np.double) 
                
                # Sliding window: Save new data and update current data window info
                new_data_len = new_data.shape[0]
                TIME_WINDOW[:-new_data_len,:] = TIME_WINDOW[new_data_len:,:]
                TIME_WINDOW[-new_data_len:,:] = new_data

                ### POSITION ESTIMATION ###
                
                # Bandpass filtering central channels
                filtered_window = GetFilteredWindow(TIME_WINDOW,bp_filter_sos,central_channels)
                
                
                # Online blink / artifact control
                if not BASELINE[-1]:
                    filtered_window, art_HM = ThresholdArtifactRemoval(filtered_window,
                                                                index_data,
                                                                len_window,
                                                                ART_THRESH,
                                                                art_HM,
                                                                baseline_dist)
                
                index_data  += len_window

                # PSD estimation
                f, Pxx = welch(x=filtered_window,
                               fs=SAMPLING_FREQ,
                               noverlap=0,
                               nperseg=SAMPLING_FREQ,
                               window='hann', 
                               axis=0)
                delta, theta = np.mean(Pxx[1:4,:],axis = 0), np.mean(Pxx[4:8,:],axis=0)
                
                # log ratio
                log_ratio = np.log10(np.mean(theta/delta))
                POS_HIST_TOTAL.append(log_ratio)
                
                # Screen and Background
                screen.blit(background,(0,0))
                
                ## BASELINE RUN: Get data to optimize screen boundaries
                if BASELINE[-1]:
                    s.fill((150,150,150,180))
                    screen.blit(s, (0,0))
                    screen.blit(baseline_txt, (w/2-baseline_txt.get_width()/2,h/8))
                    pos = float(np.random.normal(0.5,0.125,1))
                    # truncate random variable to the limits of the screen
                    if pos < 0:
                        pos = 0
                    elif pos > 1:
                        pos = 1
                    
                    if time.perf_counter()-t0 > BASELINE_TIME:
                        
                        # Save baseline
                        baseline_dict = {'DATA':df_Data[:index_data], 'POS_HIST_TOTAL':POS_HIST_TOTAL, 'PACKETS_LOST': total_packets_lost}
                        joblib.dump(baseline_dict,path_string+'/baseline_'+subj_name+'_day_'+session_n+'.mat')
                        
                        # Reset buffer
                        d.read(d.getStatus()[0])
                        
                        SUBJ_STATS[0] = np.nanmedian(list(POS_HIST_TOTAL)[baseline_position_purge:])
                        SUBJ_STATS[1] = np.nanstd(list(POS_HIST_TOTAL)[baseline_position_purge:])
                        
                        range_ratio = (SUBJ_STATS[0] + 3*  SUBJ_STATS[1]) - (SUBJ_STATS[0] - 3*  SUBJ_STATS[1])
                        min_ratio = SUBJ_STATS[0] - 3*  SUBJ_STATS[1]
                        
                        # Artifact threshold removal: get distribution and channel-specific threshold
                        baseline_dist = GetFilteredWindow(df_Data.iloc[:index_data].loc[:,np.array(EEG_NAMES)].astype(np.double).values,
                                                              bp_filter_sos,
                                                              central_channels)
                        
                        for i in range(baseline_dist.shape[1]):
                            abs_dist = np.abs(baseline_dist[:,i])
                            ART_THRESH[i] = np.percentile(abs_dist,ARTIFACT_REMOVAL_CDF_100)
                            baseline_dist[np.squeeze(np.where(abs_dist) < ART_THRESH[i]),i] = np.median(baseline_dist[:,i])

                        POS_HIST_TOTAL = deque()
                        total_packets_lost, index_data, ship_position_idx =0, 0, -1 #-1 because it will increment at the end of the loop
                        BASELINE.append(False)
                        pygame.mixer.music.play()
                
                ## EXPERIMENT: Run the story and the BCI feedback
                else:
                    # Convert log ratio into a screen position based on subject-specific
                    # statistics. Choose 3* std upwards and downwards as the ratio
                    pos = (log_ratio - min_ratio) / range_ratio
                    
                    # Midline
                    pygame.draw.line(screen, (255,100,0), (0,h/2),(w,h/2),midline_width)
                    
                    # Fill in the screen with a green background (reinforcement) depending on the position
                    s.fill((0,max(min((0.5-pos)*255*2,255),0),0,180))                        # notice the alpha value in the color
                    screen.blit(s, (0,0))
                    
                
                ## Ship drawing and history for both tasks
                # Draw trace 
                for i in range(len(POS_HIST)):
                    if i == len(POS_HIST)-1:
                        pygame.draw.line(screen, trace_color, (w/2,pos*h), 
                                          (i*w/8,POS_HIST[-1]*h), 
                                          trace_width)
                    else:
                        pygame.draw.line(screen, trace_color, ((i+1)*w/8,POS_HIST[i+1]*h), 
                                                                  (i*w/8,POS_HIST[i]*h),
                                                                  trace_width)
                
                # Update History
                POS_HIST.popleft()
                POS_HIST.append(pos)
            
                # Ship
                screen.blit(ship,(w/2, pos*h - ship_height/2))
                ship_position_idx +=1
                
                # Story / Baseline progress bar
                if not BASELINE[-1]:
                    progress_bar_height = (story_cursor_pos/ story_len_s) * h
                    pbar_color = (0,0,255)
                else:
                    progress_bar_height = ((time.perf_counter()-t0) / BASELINE_TIME) * h
                    pbar_color = (100,100,255)
                pygame.draw.rect(screen, pbar_color, (w-progress_bar_width+1, h - progress_bar_height, progress_bar_width, progress_bar_height))
                
                # Update score on experimental stage
                if not BASELINE[-1]:
                    score += max(0,int(10*(1-pos) + (pos < 0.5)*10*(1-pos)))
                    score_txt = font.render(f'Score: {score}', True, (200,200,0))
                    screen.blit(score_txt, (w*.05,h*.05))
                
                # Show battery voltage at the baseline stage (1min)
                if BASELINE[-1] and (time.perf_counter()-t0) < 30:
                    battery_level = np.round(float(df_Data.iloc[index_data-len_window,HEADERS.index('batteryV')]),1)
                    if battery_level > 2.0:
                        b_color = (150,255,150)
                    else:
                        b_color = (255,255,0)
                    battery_txt = font.render(f'{battery_level} V', True, b_color)
                    screen.blit(battery_txt, (w*.05,h*.05))
                
                
                
                # Graphical Updates and reset data window
                pygame.display.update()
                data_window = deque()

    
    else:
        extra_packets+=1

## Score screen after experiment
if scoreScreen:
    # play sound
    pygame.mixer.music.load('end.wav')
    pygame.mixer.music.play()

while scoreScreen:
    # background
    screen.blit(background,(0,0))
    
    # smiley
    screen.blit(smiley_img, (w*.5-ship_width/2,h*.05))
    
    # check quit events
    scoreScreen = PG_event_check(scoreScreen)
    
    # text
    score_txt = font.render(f'Congratulations! Final score: {score}', True, (0,200,0))
    screen.blit(score_txt, (w*.5 - score_txt.get_width()/2 ,h*.33))
    
    # update
    pygame.display.update()


# Close pygame
pygame.quit()

# Save data
experiment_dic = {'DATA':df_Data[:index_data], 
                  'POS_HIST_TOTAL':POS_HIST_TOTAL, 
                  'ART_THRESH': ART_THRESH,
                  'ART_HM_FULL': ART_HM_FULL,
                  'PACKETS_LOST': total_packets_lost
                  }
joblib.dump(experiment_dic,path_string+'/session_'+subj_name+'_day_'+session_n+'.mat')

#exit
sys.exit()