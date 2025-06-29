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

%% Read Data %%

% numpoints 144 = 24h * 6 (6 = expected entries per hour)
data = thingSpeakRead(readChannelID, 'Field', fieldID1, 'NumPoints', 144, OutputFormat='table');
t = datetime(data.Time);
y1 = data.PM10Ada;

%% Plot 
h1 = plot(t,y1,'--v', 'linewidth', 0.4, 'markersize', 3);
%set(h1, 'markerfacecolor', get(h1, 'color')) %fill smae color
title('PM1 - last 24 hours, raw');

hold on %add more plots

y2 = data.PM10Grove;

y3 = (y1+y2)/2;
h3 = plot(t,y3,'-o', 'linewidth', 2.5, 'markersize', 3);
set(h3, 'markerfacecolor', get(h3, 'color')); %fill same color

h2 = plot(t,y2,'--^', 'linewidth', 0.4, 'markersize', 3);
%set(h2, 'markerfacecolor', get(h2, 'color')) %fill same color

hold off

%% Plot's axes
endtime = t(end)
begintime = endtime - hours(24)
xlim([begintime endtime]);

xlabel('Time');
ylabel('ppm (µg/m^3)');
grid on

%% Add legend
legend({'Adafruit', 'Average', 'Grove'}, 'location', 'northwest');