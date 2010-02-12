% CompareMonCalOverTime
%
% Compare two calibrations of a display.
%
% Assumes calibration features (e.g., number of primaries, linear model dimension)
% don't change across calibrations.  This is probably safe, but an error will
% probably occur if the assumption is violated.
%
% 1/20/05	dhb, bx		Wrote it.
% 2/12/10   dhb         Don't ask for load code, just prompt for name.
%           dhb         Better plots.  And, ask for which times to compare.

%% Clear and close
clear; close all;

%% Get calibration file
loadCode = 1;
if (loadCode == 1)
    defaultFileName = 'BitsPlusScreen1';
    thePrompt = sprintf('Enter calibration filename [%s]: ',defaultFileName);
    newFileName = input(thePrompt,'s');
    if (isempty(newFileName))
        newFileName = defaultFileName;
    end
    fprintf(1,'\nLoading from %s.mat\n',newFileName);
    [cal,cals] = LoadCalFile(newFileName);
    fprintf('Calibration file %s read\n\n',newFileName);
end

if (length(cals) == 1)
	error('Only one calibration in the file.  Exiting.\n');
end

%% Print out available dates
fprintf('Calibration file contains %d calibrations\n',length(cals));
fprintf('Dates:\n');
for i = 1:length(cals)
    fprintf('\tCalibration %d, date %s\n',i,cals{i}.describe.date);
end

%% Get which to compare
defaultNow = length(cals);
defaultThen = length(cals)-1;
thenIndex = input(sprintf('Enter number of earlier calibration to compare [%d]: ',defaultThen),'s');
if (isempty(thenIndex))
    thenIndex = defaultThen;
end
if (thenIndex < 1 || thenIndex > length(cals))
    error('Calibration number out of range\n');
end

nowIndex = input(sprintf('\nEnter number of later calibration to compare [%d]: ',defaultNow),'s');
if (isempty(nowIndex))
    nowIndex = defaultNow;
end
if (nowIndex < 1 || nowIndex > length(cals))
    error('Calibration number out of range\n');
end

calNow = cals{nowIndex};
calThen = cals{thenIndex};
fprintf('\nComparing calibrations:\n');
fprintf('\t%d, %s\n',thenIndex,calThen.describe.date);
fprintf('\t%d, %s\n',nowIndex,calNow.describe.date);

%% Plot spectral power distributions.
%
% Plot as one plot if 3 or fewer primaries.
% Otherwise separate main measurements from what
% are probably the linear model correction terms.
if (size(calNow.gammaTable,2) <= calNow.nDevices)
    figure; clf; hold on
    plot(SToWls(calThen.S_device),calThen.P_device,'r');
    plot(SToWls(calNow.S_device),calNow.P_device,'g-');
    xlabel('Wavelength (nm)');
    ylabel('Power');
    title('Primaries');
else
    figure; clf;
    subplot(1,2,1); hold on
    plot(SToWls(calThen.S_device),calThen.P_device(:,1:calNow.nDevices),'r');
    plot(SToWls(calNow.S_device),calNow.P_device(:,1:calNow.nDevices),'g-');
    xlabel('Wavelength (nm)');
    ylabel('Power');
    title('Primaries');
    subplot(1,2,2); hold on
    plot(SToWls(calThen.S_device),calThen.P_device(:,calNow.nDevices+1:end),'r');
    plot(SToWls(calNow.S_device),calNow.P_device(:,calNow.nDevices+1:end),'g-');
    xlabel('Wavelength (nm)');
    ylabel('Power');
    title('Primaries (high order)');
end

%% Plot ambient
figure; clf; hold on
plot(SToWls(calThen.S_ambient),calThen.P_ambient,'r');
plot(SToWls(calNow.S_ambient),calNow.P_ambient,'g-');
xlabel('Wavelength (nm)');
ylabel('Power');
title('Ambient');

%% Explicitly compute and report ratio of R, G, and B full on spectra
rRatio = calThen.P_device(:,1)\calNow.P_device(:,1);
gRatio = calThen.P_device(:,2)\calNow.P_device(:,2);
bRatio = calThen.P_device(:,3)\calNow.P_device(:,3);
fprintf('Phosphor intensity ratios (now/then): %0.3g, %0.3g, %0.3g\n', ...
	rRatio,gRatio,bRatio);

%% Plot gamma functions
%
% Plot as one plot if 3 or fewer primaries.
% Otherwise separate main measurements from what
% are probably the linear model correction terms.
if (size(calNow.gammaTable,2) <= calNow.nDevices)
    figure; clf; hold on
    plot(calThen.gammaInput,calThen.gammaTable,'r');
    plot(calNow.gammaInput,calNow.gammaTable,'g-');
    xlabel('Input');
    ylabel('Output');
    title('Gamma');
else
    figure; clf;
    subplot(1,2,1); hold on
    plot(calThen.gammaInput,calThen.gammaTable(:,1:calNow.nDevices),'r');
    plot(calNow.gammaInput,calNow.gammaTable(:,1:calNow.nDevices),'g-');
    xlabel('Input');
    ylabel('Output');
    title('Gamma');
    subplot(1,2,2); hold on
    plot(calThen.gammaInput,calThen.gammaTable(:,calNow.nDevices+1:end),'r');
    plot(calNow.gammaInput,calNow.gammaTable(:,calNow.nDevices+1:end),'g-');
    xlabel('Input');
    ylabel('Output');
    title('Gamma (high order)');
end

%% Let's print some luminance information
load T_xyzJuddVos;
T_xyz = SplineCmf(S_xyzJuddVos,683*T_xyzJuddVos,calThen.S_device);
lumsThen = T_xyz(2,:)*calThen.P_device;
maxLumThen = sum(lumsThen);
lumsNow = T_xyz(2,:)*calNow.P_device;
maxLumNow = sum(lumsNow);
fprintf('Maximum luminance: then %0.3g; now %0.3g\n',maxLumThen,maxLumNow);
minLumThen = T_xyz(2,:)*calThen.P_ambient;
minLumNow = T_xyz(2,:)*calNow.P_ambient;
fprintf('Minimum luminance: then %0.3g; now %0.3g\n',minLumThen,minLumNow);
