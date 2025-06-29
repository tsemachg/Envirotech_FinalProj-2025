% Template MATLAB code for visualizing data from a channel as a 2D line
% plot using PLOT function.

% Prior to running this MATLAB code template, assign the channel variables.
% Set 'readChannelID' to the channel ID of the channel to read from. 
% Also, assign the read field ID to 'fieldID1'. 

% TODO - Replace the [] with channel ID to read data from:
readChannelID = 2997781;
% TODO - Replace the [] with the Field ID to read data from:
fieldID1 = [1,2];

% Channel Read API Key 
% If your channel is private, then enter the read API
% Key between the '' below: 
%readAPIKey = 'KCH6O64H275JO9X8'; %% public no need 

%% Read Data %%

% numpoints 288 = 48h * 6 (6 = expected entries per hour)
data = thingSpeakRead(readChannelID, 'Field', fieldID1, 'NumPoints', 288, OutputFormat='table');
t = datetime(data.Time);
d = data.NotecardTempC;


%% Visualize Data %%

h1 = plot(t, d,'-','color','r', 'linewidth', 1.5)% 2, 'markersize', 4)
%set(h1, 'markerfacecolor', get(h1, 'color'));
title('Notecard Temperature - 48h');

%% Plot's axes
endtime = t(end);
begintime = endtime - hours(48);
xlim([begintime endtime]);
xlabel('Time');
ylabel('Degrees (°C)');
grid on