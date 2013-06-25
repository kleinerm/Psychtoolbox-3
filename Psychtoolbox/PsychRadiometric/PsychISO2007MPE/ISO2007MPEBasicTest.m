% ISO2007MPEBasicTest
%
% ****************************************************************************
% IMPORTANT: Before using the ISO2007MPE routines, please see the notes on usage
% and responsibility in PsychISO2007MPE/Contents.m (type "help PsychISO2007MPE"
% at the Matlab prompt.
% ****************************************************************************
%
% Test code for our implementation of ANSI Z136.1-2007. Reproduces many figures from the
% standard.
%
% 2/22/13  dhb  Wrote it.

%% Clear and close
clear; close all;

%% Read in and plot the spectral functions used by the standard
S = [200 1 1301];
[wls,weightingR,weightingA,weightingS,wls_R,rawWeigtingR,wls_A,rawWeightingA,wls_S,rawWeightingS] = ISO2007MPEGetWeighings(S);

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

% figure; clf; hold on
% plot(theStimulusSizesMrad,T2Sec,'ro','MarkerSize',8,'MarkerFaceColor','r');
% xlabel('Stimulus Size (mrad)');
% ylabel('T2 (sec)');
% xlim([0 max(theStimulusSizesMrad)]);
% ylim([0 100]);
% title('Figure 9b: Test of AnsiZ136MPEComputeT2');
% grid on
