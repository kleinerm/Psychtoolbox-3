% ISO2007MPEBasicTest
%
% ****************************************************************************
% IMPORTANT: Before using the ISO2007MPE routines, please see the notes on usage
% and responsibility in PsychISO2007MPE/Contents.m (type "help PsychISO2007MPE"
% at the Matlab prompt.
% ****************************************************************************
%
% Test code for our implementation of the ISO 2007 broadband MPE standard.
%
% 6/26/13  dhb  Wrote it.

%% Clear and close
clear; close all;

%% Wavelength sampling.
%
% Easiest to cover the whole range covered by the standard,
% that way we don't have to think.
S = [200 1 1301];

%% Load CIE functions.   
load T_xyz1931
T_xyz = SplineCmf(S_xyz1931,683*T_xyz1931,S);

%% Load in a test spectrum
%
% This is a bright sunlight measured through a window in Philly.
% We only have measurements between 380 and 780 nm.  In this
% example, we zero extend when we spline to the whole range.
load spd_phillybright
spd_phillybright = SplineSpd(S_phillybright,spd_phillybright,S,0);
photopicLuminancePhillyBrightCdM2 = T_xyz(2,:)*spd_phillybright;
figure; 
plot(SToWls(S),spd_phillybright,'r','LineWidth',2);
xlabel('Wavelength (nm)');
ylabel('Radiance (Watts/[sr-m2-wlinterval]');

%% Specify stimulus parameters
stimulusDiameterDegrees = 10;
stimulusAreaDegrees2 = pi*((stimulusDiameterDegrees/2)^2);
stimulusDurationSecs = 60*60;

%% Read in and plot the spectral functions used by the standard
[wls,weightingR,weightingA,weightingS,wls_R,rawWeightingR,wls_A,rawWeightingA,wls_S,rawWeightingS] = ISO2007MPEGetWeighings(S);

figure; clf; set(gcf,'Position',[400 500 1400 550]);
subplot(1,3,1); hold on
plot(wls_R,rawWeightingR,'r','LineWidth',3);
plot(wls,weightingR,'b:','LineWidth',2);
xlabel('Wavelength (nm)')
ylabel('R_lambda')
title('Thermal Hazard Function');
xlim([200 1500]);
subplot(1,3,2); hold on
plot(wls_A,rawWeightingA,'r','LineWidth',3);
plot(wls,weightingA,'b:','LineWidth',2);
xlabel('Wavelength (nm)')
ylabel('A_lambda')
title('Aphakic Photochemical Hazard Function');
xlim([200 1500]);
subplot(1,3,3); hold on
plot(wls_S,rawWeightingS,'r','LineWidth',3);
plot(wls,weightingS,'b:','LineWidth',2);
xlabel('Wavelength (nm)')
ylabel('S_lambda')
title('UV Radiation Hazard Function');
xlim([200 1500]);

%% Corenal irradiance weighted UV limit
[val1_UWattsPerCm2,limit1_UWattsPerCm2] = ISO2007MPEComputeType1ContinuousCornealUVWeightedValue(...
    S,spd_phillybright,weightingS,stimulusDurationSecs,stimulusAreaDegrees2);
fprintf('  * Type 1 continuous corneal irradiance UV weighted (5.4.1.1)\n');
fprintf('    * Value: %0.3f, limit %0.3f (uWatts/cm2)\n',val1_UWattsPerCm2,limit1_UWattsPerCm2);

%% Corenal irradiance uweighted UV limit
[val2_UWattsPerCm2,limit2_UWattsPerCm2] = ISO2007MPEComputeType1ContinuousCornealUVUnweightedValue(...
S,spd_phillybright,stimulusDurationSecs,stimulusAreaDegrees2);
fprintf('  * Type 1 continuous corneal irradiance UV unweighted (5.4.1.2)\n');
fprintf('    * Value: %0.3f, limit %0.3f (uWatts/cm2)\n',val2_UWattsPerCm2,limit2_UWattsPerCm2);

%% Retinal irradiance weighted aphakic limit
[val3_UWattsPerCm2,limit3_UWattsPerCm2] = ISO2007MPEComputeType1ContinuousRetIrradiancePCWeightedValue(...
    S,spd_phillybright,weightingA,stimulusDurationSecs);
fprintf('  * Type 1 continuous aphakic retinal illumiance weighted (5.4.1.3.a)\n');
fprintf('    * Value: %0.3f, limit %0.3f (uWatts/cm2)\n',val3_UWattsPerCm2,limit3_UWattsPerCm2);

%% Radiance weighted aphakic limit
[val4_UWattsPerSrCm2,limit4_UWattsPerSrCm2] = ISO2007MPEComputeType1ContinuousRadiancePCWeightedValue(...
    S,spd_phillybright,weightingA,stimulusDurationSecs);
fprintf('  * Type 1 continuous aphakic radiance weighted (5.4.1.3.b)\n');
fprintf('    * Value: %0.3f, limit %0.3f (uWatts/[sr-cm2])\n',val4_UWattsPerSrCm2,limit4_UWattsPerSrCm2);

%% Corneal irradiance unweighted IR limit
[val5_UWattsPerCm2,limit5_UWattsPerCm2] = ISO2007MPEComputeType1ContinuousCornealIRUnweightedValue(...
    S,spd_phillybright,stimulusDurationSecs,stimulusAreaDegrees2);
fprintf('  * Type 1 continuous corneal irradiance IR unweighted (5.4.1.3.b)\n');
fprintf('    * Value: %0.3f, limit %0.3f (uWatts/[sr-cm2])\n',val5_UWattsPerCm2,limit5_UWattsPerCm2);

%% Retinal irradiance weighted thermal limit
[val6_UWattsPerCm2,limit6_UWattsPerCm2] = ISO2007MPEComputeType1ContinuousRetIrradianceTHWeightedValue(...
    S,spd_phillybright,weightingR,stimulusDurationSecs);
fprintf('  * Type 1 continuous thermal retinal illumiance weighted (5.4.1.3.a)\n');
fprintf('    * Value: %0.3f, limit %0.3f (uWatts/cm2)\n',val6_UWattsPerCm2,limit6_UWattsPerCm2);

%% Radiance weighted thermal limit
[val7_UWattsPerSrCm2,limit7_UWattsPerSrCm2] = ISO2007MPEComputeType1ContinuousRadianceTHWeightedValue(...
    S,spd_phillybright,weightingR,stimulusDurationSecs);
fprintf('  * Type 1 continuous thermal radiance weighted (5.4.1.3.b)\n');
fprintf('    * Value: %0.3f, limit %0.3f (uWatts/[sr-cm2])\n',val7_UWattsPerSrCm2,limit7_UWattsPerSrCm2);