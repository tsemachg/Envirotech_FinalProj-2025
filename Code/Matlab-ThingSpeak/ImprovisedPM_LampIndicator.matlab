%% An attempt to try and create imrovised lamp indicatetor, uilamp not supported at thingspeak "This 
%  functionality is not available on remote platforms". The below code was created manually after lots 
%  of experimenting. (I'm not a MATLAB user, I did my best)

% planning - create plot of circle shape / single scatter - large markers; can search if text inside 
% plot is possible like python/R. Then create the desired PM1=[], PM2.5=[]... total=[]
%  ** Create function for the condition of plot according to value, wich accepts parameter of 
%     size, position and text. 


%% ---------------------------- READ DATA ------------------------- %%
readChannelID = 2997781;
fieldID1 = [1,3:8];
%% read only last entry
data = thingSpeakRead(readChannelID, 'Field', fieldID1, 'NumPoints', 1, OutputFormat='table');

%% Assign
t = datetime(data.Time);
lastt = string(t);
pm10ada = data.PM10Ada;
pm10grove = data.PM10Grove;
pm25ada = data.PM25Ada;
pm25grove = data.PM25Grove;
pm100ada = data.PM100Ada;
pm100grove = data.PM100Grove;

pm10 = (data.PM10Ada + data.PM10Grove)/2;
pm25 = (data.PM25Ada + data.PM25Grove)/2;
pm100 = (data.PM100Ada + data.PM100Grove)/2;


%% ---------------------------- PLOT ------------------------- %%

%% background
figure;
xlim([0 1]);
ylim([0 1]);
hold on

%% --------------- positions -------------- %%
mpos = [0.58 0.88]; % main xy position
apos = [0.78 0.57]; % adafruit xy position
gpos = [0.98 0.57]; % grove xy position
p10pos = [0.48 0.41]; % pm1 xy position
p25pos = [0.48 0.26]; % pm2.5 xy position
p100pos = [0.48 0.11]; % pm10 xy position
%% l = legend
llamp = 0; % legend lamps x pos
ltxt = 0.05; % legend text begin position - left aligned
lgreen = 0.83; % only y
lorange = lgreen-0.15; % only y
lred = lorange-0.15; % only y
lerr = lred-0.15; % only y


%% ---------------- Shapes ------------------- %%
%          canBeShape  [xpos,ypos,xdim,ydim]
%annotation('textbox', [0.5, 0.5, 0.2, 0.2], 'string', 'whoa!');
annotation('rectangle', [0.07, 0.36, 0.36, 0.6], 'color', [0.5 0.5 0.5]); %% upper rectangle
annotation('rectangle', [0.07, 0.06, 0.36, 0.25],'color', [0.5 0.5 0.5]); %% lower rectangle

%% ---------------- Texts ------------------- %%

%text(x,y, '<inputText>', 'color', '<col>', 'fontsize', <num>, 'fontweight', 'normal/bold', 'hotizontalalignmetn', 'left/center/right');
%% left side
text(mpos(1), mpos(2), {'Danger', 'Level:'}, 'fontweight', 'bold', 'fontsize', 20, 'horizontalalignment', 'center');
text(apos(1), apos(2), 'Adafruit', 'fontsize', 13, 'horizontalalignment', 'center');
text(gpos(1), gpos(2), 'Grove', 'fontsize', 13, 'horizontalalignment', 'center');
text(p10pos(1), p10pos(2), 'PM1:', 'fontsize', 14);
text(p25pos(1), p25pos(2), 'PM2.5:','fontsize', 14);
text(p100pos(1), p100pos(2), 'PM10:', 'fontsize', 14);
text(0.58, -0.067, '*Unhealthy for sensitive groups', 'fontsize', 9); % remark *

%% right side (legend)
text(-0.06, 0.965, 'Air Quality:', 'fontweight', 'bold', 'fontsize', 13); % title up
text(ltxt, lgreen, '- Good to go!', 'fontsize', 11);
text(ltxt, lorange, '- Moderate*', 'fontsize', 11);
text(ltxt, lred, '- Unhealthy', 'fontsize', 11);
text(ltxt, lerr, '- Error in Sensor', 'fontsize', 10);
text(-0.06, 0.17, 'Last update:', 'fontweight', 'bold', 'fontsize', 13); % title low

text(-0.035, 0.04, lastt, 'fontsize', 10); % Date time - last update


%% function to control lamp color according to ppm concentration following:
% https://www.epa.gov/sites/default/files/2016-04/documents/2012_aqi_factsheet.pdf
function c = decideColor(x)
    if x>=0 && x<=35
        c = [0 0.8 0]; % green
    elseif x>35 && x<=55
        c = [0.9 0.5 0]; % orange
    elseif x>55 
        c = [0.8 0 0]; % red
    else
        c = [0.4 0.4 0.4]; % gray = error
    end
end

%% function to create lamps
function createLamp(x,y,size,col,lw)
    cmax = 1; % color max value
    cmin = 0; % color min value
    
    lamp = scatter(x, y, size, 'filled', 'markerfacecolor', decideColor(col)); % create "lamp"
    edge = get(lamp, 'markerfacecolor') + [0.4 0.4 0.4]; % brighter edges, can invert colors with "-" instead of "+" (and changing more)
    edge(edge>cmax) = cmax; % do not exceed valid value range
    %edge(edge<cmin) = cmin;
    set(lamp, 'linewidth',lw, 'markeredgecolor', edge); %fill same color

end

lwsmall = 2;

%% -------------  lamps ----------  %%
%% permanent values for legend
green = 2;
orange = 40;
red = 60;
err = -1;

%% color for main lamp
% if 1 of the sensors && average are higher than value, display it
% take into consideration 3 PM sizes, if one is danger the MAIN will display?
c = pm10;
%if 
%    a
%end

%% ---- leftside lamps
createLamp(0.88,mpos(2),1500,c,3); % main lamp
createLamp(apos(1),p10pos(2),100,pm10ada,lwsmall); % ada pm1 lamp
createLamp(gpos(1),p10pos(2),100,pm10grove,lwsmall); % grove pm1 lamp
createLamp(apos(1),p25pos(2),100,pm25ada,lwsmall); % ada pm25 lamp
createLamp(gpos(1),p25pos(2),100,pm25grove,lwsmall); % grove pm25 lamp
createLamp(apos(1),p100pos(2),100,pm100ada,lwsmall); % ada pm10 lamp
createLamp(gpos(1),p100pos(2),100,pm100grove,lwsmall); % grove pm10 lamp

%% ---- rightside lamps (legend)
createLamp(llamp,lgreen,100,green,lwsmall); % legend green lamp
createLamp(llamp,lorange,100,orange,lwsmall); % legend orange lamp
createLamp(llamp,lred,100,red,lwsmall); % legend red lamp
createLamp(llamp,lerr,100,err,lwsmall); % legend err lamp


set(gca, 'xcolor', [0.7,0.7,0.7], 'ycolor', [0.7,0.7,0.7]); % for help,     delete later

box on % help visualize
grid on %to help visualize

%% Un comment to hide axes
axis off
