% Prior to running this MATLAB code template, assign the channel variables.
% Set 'readChannelID' to the channel ID of the channel to read from. 
% Also, assign the read field ID to 'fieldID1'. 

% TODO - Replace the [] with channel ID to read data from:
readChannelID = 2997781;
% TODO - Replace the [] with the Field ID to read data from:
fieldID1 = [1,3,6];

% Channel Read API Key 
% If your channel is private, then enter the read API
% Key between the '' below: 
readAPIKey = 'KCH6O64H275JO9X8';

%% Read Data %%

data = thingSpeakRead(readChannelID, 'Field', fieldID1, 'NumPoints', 60, 'ReadKey', readAPIKey, OutputFormat='table');
t = datetime(data.Time);
y1 = data.PM10Ada;

%% Plot 
h1 = plot(t,y1,'-s', 'linewidth', 0.5)
%set(h1, 'markerfacecolor', get(h1, 'color')) %fill smae color
title('PM1')

hold on %add more plots

y2 = data.PM10Grove;

y3 = (y1+y2)/2;
h3 = plot(t,y3,'-o', 'linewidth', 2)
set(h3, 'markerfacecolor', get(h3, 'color')) %fill same color

h2 = plot(t,y2,'-^', 'linewidth', 0.5)
%set(h2, 'markerfacecolor', get(h2, 'color')) %fill same color

hold off

xlabel('Time')
ylabel('ppm (µm/m^3)')

%% Add legend
legend('Adafruit', 'Average', 'Grove')