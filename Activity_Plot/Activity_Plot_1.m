%%
%
%   EEG reading: Spectrogram, Pwelch and Voltage plotting
%
%   Part1 - Initialization
%
%%

%  15 seconds long EEG buffer for channel O1 and Fp1
EEG_O1  = ones(1, 7500);
EEG_Fp1 = ones(1, 7500);

EEG_O1_minute  = ones(1, 30000);

%  15 seconds long filtered data buffer
yPlot1 = ones(1,7500);
yPlot2 = ones(1,7500);

%  15 seconds of xPlot
xPlot = [1:7500];

%  Filters
Fs = 500; % Cognionics uses 500Hz sampling frequency
Nyq = Fs/2; % Nyquist frequency
[myButter.b, myButter.a] = butter(4, [1 80]/Nyq,'bandpass');
Wo = 50/Nyq;
BW = Wo/20;
[myButter.d, myButter.c] = iirnotch(Wo,BW);

%  Pwelch parameters
window = length(yPlot1)/8; % 8 segments
noverlap = window/2;       % 50% overlap
nfft = length(yPlot1);     % number of Data points for the fx

%% MATLAB PWELCH STANDARD
%By default, x is divided into the longest possible segments to obtain as close to but not exceed 8 segments with 50% 
%overlap. Each segment is windowed with a Hamming window. The modified periodograms are averaged to obtain the PSD estimate. 
%If you cannot divide the length of x exactly into an integer number of segments with 50% overlap, x is truncated accordingly.

