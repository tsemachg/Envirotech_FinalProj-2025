% Prior to running this MATLAB code template, assign the channel variables.
% Set 'readChannelID' to the channel ID of the channel to read from. 
% Also, assign the read field ID to 'fieldID1'. 

% TODO - Replace the [] with channel ID to read data from:
readChannelID = 2997781;
% TODO - Replace the [] with the Field ID to read data from:
fieldID1 = [1,3:8];

% Channel Read API Key 
% If your channel is private, then enter the read API
% Key between the '' below: 

%% Read Data %%

% numpoints 288 = 48h * 6 (6 = expected entries per hour)
data = thingSpeakRead(readChannelID, 'Field', fieldID1, 'NumPoints', 288, OutputFormat='table');
t = datetime(data.Time);

%% Assign, calculate mean
pm10 = (data.PM10Ada + data.PM10Grove)/2;
pm25 = (data.PM25Ada + data.PM25Grove)/2;
pm100 = (data.PM100Ada + data.PM100Grove)/2;

%% Plot smoothed data
h1 = plot(t,smoothdata(pm10, 'gaussian', 10),':', 'linewidth', 1.5);
title('Particulate Matter, Smoothed - last 48 hours');

hold on

h2 = plot(t,smoothdata(pm25, 'gaussian', 10),'-.', 'linewidth', 1.5);

h3 = plot(t,smoothdata(pm100, 'gaussian', 10),'-', 'linewidth', 1.5);

hold off

%% Plot's axes
endtime = t(end);
begintime = endtime - hours(48);
xlim([begintime endtime]);

xlabel('Time');
ylabel('ppm (µg/m^3)');
%ax = gca;
%ax.YGrid = 'on';
grid on

%% Add legend
legend({'PM1', 'PM2.5', 'PM10'}, 'location', 'northwest');