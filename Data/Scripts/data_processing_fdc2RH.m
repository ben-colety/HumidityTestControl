clear all;

%import data
FileName = uigetfile('.csv')
fdc_data = readtable(FileName);
fdc_data = rmmissing(fdc_data);
% FileName = uigetfile('.csv')
% bme_data = readtable(FileName);
% bme_data = rmmissing(bme_data);
FileName = uigetfile('.mat');
load(FileName);

%find min max of bme
%[bme_min, bme_max] = bounds(bme_data.RH);

% lets try with FDC calibration


% Remove outliers
fdc_data = rmoutliers(fdc_data,"movmedian",1000,"DataVariables","humidity");
%%

fdc_data.RH = rescale(fdc_data.humidity,"InputMin", sensor.wet_dig, "InputMax",sensor.dry_dig);

%invert fdc curve
fdc_data.RH = (1-fdc_data.RH)*100;

% avg_temp = mean(bme_data.Temp)


%get RH from FDC curve
%fdc_data.RH = rescale(fdc_data.RH, bme_min, bme_max);
% %% 
% plot(fdc_data.time-min(fdc_data.time), fdc_data.RH)
% % Create ylabel
% ylabel({'Relative Humidity [%]'});
% 
% % Create xlabel
% xlabel({'time [ms]'});
% ylim([0 100])
% % Create title
% title({'P14 Rapid-2 Relative Humidity Response'});
% dim = [0.457142857142857 0.169047620466779 0.414285702471222 0.0714285700094133];
% str = ['Average Temperature : ' num2str(round(avg_temp,2)) '^{o}C'];
% annotation('textbox',dim,'String',str,'FitBoxToText','on');
%%
fdc_noise = fdc_data(100:400,["time" "RH"]);
fdc_noise.time = fdc_noise.time - min(fdc_noise.time);
fdc_noise_smooth = smoothdata(fdc_noise,"movmean","SmoothingFactor",0.25,...
    "DataVariables","RH");
std_dev = std(fdc_noise.RH - fdc_noise_smooth.RH)