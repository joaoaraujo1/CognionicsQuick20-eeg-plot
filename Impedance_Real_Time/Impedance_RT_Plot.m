%%
%
%   Impedance check GUI in real time for Cognionics Quick20
%
%   Jo?o Ara?jo, 2017
%
%%
% close all;
% 
% threshold = 4000;
% Impedance_values = ones(1,20);
% Impedance_values(4) = 5000;


%Impedance threshold set GUI
brain.impSetTitle=uicontrol('Style','text',...
'units','normalized',...
'position',[.775 .60 .20 .07],...
'String','Impedance threshold (kOhms)',...
'FontWeight','bold');

brain.impSetTxt=uicontrol('Style','edit',...
'units','normalized',...
'position',[.775 .555 .20 .05],...
'String',threshold,...
'FontWeight','normal');

brain.impSetBut=uicontrol('Style','pushbutton',...
'units','normalized',...
'position',[.775 .495 .20 .05],...
'String','Set Threshold','FontWeight','bold',...
'callback','threshold = str2double(get(brain.impSetTxt, ''String''));');

% Channel 1
if(Impedance_values(1) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(1)-rem(Impedance_values(1),1));
channel_txt = ['F7 - ' str_imp];
brain.F7=uicontrol('Style','text',...
'units','normalized',...
'position',[.06 - .05, .7 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 2
if(Impedance_values(2) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(2)-rem(Impedance_values(2),1));
channel_txt = ['Fp1 - ' str_imp];
brain.Fp1=uicontrol('Style','text',...
'units','normalized',...
'position',[.20 - .05, .9 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 3
if(Impedance_values(3) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(3)-rem(Impedance_values(3),1));
channel_txt = ['Fp2 - ' str_imp];
brain.Fp2=uicontrol('Style','text',...
'units','normalized',...
'position',[.50 - .05, .9 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 4
if(Impedance_values(4) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(4)-rem(Impedance_values(4),1));
channel_txt = ['F8 - ' str_imp];
brain.F8=uicontrol('Style','text',...
'units','normalized',...
'position',[.65 - .05, .7 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 5
if(Impedance_values(5) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(5)-rem(Impedance_values(5),1));
channel_txt = ['F3 - ' str_imp];
brain.F3=uicontrol('Style','text',...
'units','normalized',...
'position',[.20 - .05, .7 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 6
if(Impedance_values(6) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(6)-rem(Impedance_values(6),1));
channel_txt = ['Fz - ' str_imp];
brain.Fz=uicontrol('Style','text',...
'units','normalized',...
'position',[.35 - .05, .7 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 7
if(Impedance_values(7) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(7)-rem(Impedance_values(7),1));
channel_txt = ['F4 - ' str_imp];
brain.F4=uicontrol('Style','text',...
'units','normalized',...
'position',[.50 - .05, .7 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 8
if(Impedance_values(8) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(8)-rem(Impedance_values(8),1));
channel_txt = ['C3 - ' str_imp];
brain.C3=uicontrol('Style','text',...
'units','normalized',...
'position',[.20 - .05, .5 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 9
if(Impedance_values(9) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(9)-rem(Impedance_values(9),1));
channel_txt = ['Cz - ' str_imp];
brain.Cz=uicontrol('Style','text',...
'units','normalized',...
'position',[.35 - .05, .5 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 10
if(Impedance_values(10) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(10)-rem(Impedance_values(10),1));
channel_txt = ['P8 - ' str_imp];
brain.P8=uicontrol('Style','text',...
'units','normalized',...
'position',[.65 - .05, .3 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 11
if(Impedance_values(11) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(11)-rem(Impedance_values(11),1));
channel_txt = ['P7 - ' str_imp];
brain.P7=uicontrol('Style','text',...
'units','normalized',...
'position',[.06 - .05, .3 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 12
if(Impedance_values(12) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(12)-rem(Impedance_values(12),1));
channel_txt = ['Pz - ' str_imp];
brain.Pz=uicontrol('Style','text',...
'units','normalized',...
'position',[.35 - .05, .3 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 13
if(Impedance_values(13) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(13)-rem(Impedance_values(13),1));
channel_txt = ['P4 - ' str_imp];
brain.P4=uicontrol('Style','text',...
'units','normalized',...
'position',[.50 - .05, .3 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 14
if(Impedance_values(14) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(14)-rem(Impedance_values(14),1));
channel_txt = ['T3 - ' str_imp];
brain.T3=uicontrol('Style','text',...
'units','normalized',...
'position',[.06 - .05, .5 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 15    
if(Impedance_values(15) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(15)-rem(Impedance_values(15),1));
channel_txt = ['P3 - ' str_imp];
brain.P3=uicontrol('Style','text',...
'units','normalized',...
'position',[.20 - .05, .3 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 16
if(Impedance_values(16) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(16)-rem(Impedance_values(16),1));
channel_txt = ['O1 - ' str_imp];
brain.O1=uicontrol('Style','text',...
'units','normalized',...
'position',[.20 - .05, .075 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 17
if(Impedance_values(17) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(17)-rem(Impedance_values(17),1));
channel_txt = ['O2 - ' str_imp];
brain.O2=uicontrol('Style','text',...
'units','normalized',...
'position',[.50 - .05, .075 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 18
if(Impedance_values(18) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(18)-rem(Impedance_values(18),1));
channel_txt = ['C4 - ' str_imp];
brain.C4=uicontrol('Style','text',...
'units','normalized',...
'position',[.50 - .05, .5 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

% Channel 19
if(Impedance_values(19) < threshold) c_color = 'g';
else c_color = 'r';
end
str_imp = num2str(Impedance_values(19)-rem(Impedance_values(19),1));
channel_txt = ['T4 - ' str_imp];
brain.T4=uicontrol('Style','text',...
'units','normalized',...
'position',[.65 - .05, .5 - .05, .075, .075],...
'String',channel_txt,'BackgroundColor',c_color);

drawnow;




