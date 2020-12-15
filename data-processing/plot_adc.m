% fileId = fopen('../data/samples_sin_ampl50mV_freq_2hz_offset0.txt','r');
% fileId = fopen('../data/samples_square_ampl50mv_freq_2hz_offset0.txt','r');
fileId = fopen('../data/samples_triangle_ampl100mv_freq_1hz_offset0.txt','r');

CONFIG = textscan(fileId,'ADS1115 CONFIG: %xu16',1);
data   = textscan(fileId,'XU4 Microsecond: %u32, ADC Sample: %d16');

settingsPGA        = [2/3,1,2,4,8,16,16,16];
settingsResolution = [0.1875,0.125,0.0625,0.03125,0.015625,0.0078125,0.0078125,0.0078125];
settingsDR         = [8,16,32,64,128,250,475,860];
PGA = bitshift(bitand(CONFIG{1,1},0x0E00,'uint16'),-9);
DR  = bitshift(bitand(CONFIG{1,1},0x00E0,'uint16'),-5);
gainPGA     = settingsPGA(PGA+1);
vResolution = settingsResolution(PGA+1);
dataRate    = settingsDR(DR+1);
fprintf('Data rate: %d SPS\n',dataRate);
fprintf('PGA gain: %d\n',gainPGA);
fprintf('Voltage resolution: %d mV\n',vResolution);

voltage = double(data{1,2})*vResolution;
fprintf('Total number of samples: %d\n',size(voltage,1));

figure(1);
plot(voltage);
ax = gca;
ax.FontSize = 14;
xlabel('n','FontSize',16);
ylabel('voltage (mV)','FontSize',16);
