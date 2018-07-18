%%
%
%   EEG reading: Spectrogram, Pwelch and Voltage plotting
%
%   Part2 - Main Function
%
%   Joao Araujo, 2017
%
%%

%% Buffer update and signal filtering

%  EEG data from channel 1-19
Data = Data(:,2:19);

EEG_O1 = circshift(EEG_O1, [0,-125]); % moving window of .25 seconds at 500Hz
EEG_O1(: , end - 125 + 1 : end ) = Data(:,16); % Channel 16 = O1

% EEG_O1 = [EEG_O1(:,126:end) Data(:,16)']; % Channel 16 = O1

EEG_O1_minute = circshift(EEG_O1_minute, [0,-125]); % moving window of .25 seconds at 500Hz
EEG_O1_minute(: , end - 125 + 1 : end ) = Data(:,16); % Channel 16 = O1

EEG_Fp1 = circshift(EEG_Fp1, [0,-125]); % moving window of .25 seconds at 500Hz
EEG_Fp1(: , end - 125 + 1 : end ) = Data(:,2); % Channel 2 = Fp1

%  Filter for alpha O1
[myButter.b1, myButter.a1] = butter(4, [3 20]/Nyq,'bandpass');

%  Filter data from O1 and store it on yPlot1
yPlot1 = filtfilt(myButter.b, myButter.a, EEG_O1);
yPlot1 = filter(myButter.d, myButter.c, yPlot1);

%  Alpha filter data from O1 and store it on yPlot11
yPlot11 = filtfilt(myButter.b1, myButter.a1, EEG_O1);
yPlot11 = filter(myButter.d, myButter.c, yPlot11);

%  Filter data from O1 activity recorded in 1 minute
yPlot_minute = filtfilt(myButter.b, myButter.a, EEG_O1_minute);
yPlot_minute = filter(myButter.d, myButter.c, yPlot_minute);

%  Filter data from Fp1 and store it on yPlot2
yPlot2 = filtfilt(myButter.b, myButter.a, EEG_Fp1);
yPlot2 = filter(myButter.d, myButter.c, yPlot2);


%% Plotting

%  Fp1 activity  
subplot(4,1,1)     
plot([1:length(xPlot)]/Fs, yPlot2)
ylim([-.0005, .0005])
ylabel('V');
title('Channel Fp1 - Activity over Time (s)')
set(gca,'fontsize',14)

%  O1 alpha filtered activity
subplot(4,1,2)  
plot([1:length(xPlot)]/Fs, yPlot11)
ylim([-.000080,.000080]);
ylabel('V');
title('Channel O1 - Activity over Time (s)')
set(gca,'fontsize',14)

%  O1 PWelch (regular filter)
subplot(2,2,3);
[pxx,fx] = pwelch(sqrt(yPlot1), hamming(window),noverlap,7500,Fs);           % 15 second buffer - alpha demo
%[pxx,fx] = pwelch(sqrt(yPlot_minute), hamming(30000/8),30000/16,30000,Fs);  % 1 minute buffer SSVEP demo
semilogy(fx,pxx,'r');
title('Channel O1 - Power over Frequency');
set(gca,'fontsize',14);
%xlim([6,10]); % SSVEP demo
xlim([1,50]);  % alpha demo
ylim([10^-8, 10^-6]);
xlabel('Hz');
ylabel('V/Hz^1^/^2');
grid on; grid minor;

%  O1 Spectrogram (regular filter) 15sec
subplot(2,2,4);
%spectrogram(yPlot_minute,hamming(125 + 62),62,[],Fs,'yaxis');
spectrogram(yPlot1,hamming(125 + 62),62,[],Fs,'yaxis');
caxis([-120, -110]);
title('Channel O1 - Frequency Power over Time');
ylim([2, 40]);
set(gca,'fontsize',14);
ylabel('Hz');
colorbar

drawnow