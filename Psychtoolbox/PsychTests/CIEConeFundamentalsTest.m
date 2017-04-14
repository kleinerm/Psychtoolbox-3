% CIEConeFundamentalsTest
%
% This program tests the CIE cone fundamentals routines, and demonstrates their use.
%
% The goal here is to find parameters that reproduce a set of cone
% fundamentals from their underlying parts.  That would then let
% one vary the parameters of the parts, to get different theoretically
% specfied cone fundamentals.
%
% This shows that the standard does an excellent job of reconstructing
% the Stockman/Sharpe 2-degree and 10 degree fundamentals if one starts
% with the tabulated LMS absorbances.  The agreement is less perfect
% if one uses the nomogram and recommended lambda-max values to
% generate the absorbances.  See comment on this point in
% StockmanSharpeNomogram.m.
%
% The code here is closely related (and uses) a more general set of code
% for setting parameters for photoreceptors and computing quantal
% sensitivities. See:
%    DefaultPhotoreceptors, FillInPhotoreceptors, PrintPhotoreceptors,IsomerizationsInDishDemo
%    IsomerizationsInEyeDemo, ComputeCIEConeFundamentals,
%    ComputeRawConeFundamentals, CIEConeFundamentalsFieldSizeTest.
%
% 8/11/11  dhb  Wrote it
% 8/14/11  dhb  Clean up a little.
% 12/16/12 dhb  Added test for rods.
% 08/10/13 dhb  Better integration with the photoreceptor struct code.
% 03/14/14 dhb  Add Smith-Pokorny to 2 degree plot, for comparison.
% 3/31/17  ms   Added documentation of the adjIndDiffParams output args.

%% Clear
clear; close all;

%% Parameters
DUMPFIGURES = 0;
S = WlsToS((390:5:780)');

%% Low end of log plot scale
lowEndLogPlot = -4;

%% Get tabulated fundamentals and normalize. These correspond
% to a 32 year old observer with a small (<= 3 mm) pupil.
%
% The call to QuantaToEnergy to accomplish an energy to quanta transform is not an error.
% Rather, that routine converts spectra, but here we are converting sensitivities, so its
% the inverse we want.
targetRaw = load('T_cones_ss2');
T_targetEnergy = SplineCmf(targetRaw.S_cones_ss2,targetRaw.T_cones_ss2,S,2);
T_targetQuantal2 = QuantaToEnergy(S,T_targetEnergy')';

targetRaw = load('T_cones_ss10');
T_targetEnergy = SplineCmf(targetRaw.S_cones_ss10,targetRaw.T_cones_ss10,S,2);
T_targetQuantal10 = QuantaToEnergy(S,T_targetEnergy')';

for i = 1:3
    T_targetQuantal2(i,:) = T_targetQuantal2(i,:)/max(T_targetQuantal2(i,:));
    T_targetQuantal10(i,:) = T_targetQuantal10(i,:)/max(T_targetQuantal10(i,:));
end

%% Compute 2 degree and plot
%
% Also add Smith-Pokorny
load T_cones_sp
T_predictQuantalCIE2 = ComputeCIEConeFundamentals(S,2,32,3);
T_predictQuantalCIE2Nomo = ComputeCIEConeFundamentals(S,2,32,3,[558.9 530.3 420.7]');
figure; clf; hold on
position = get(gcf,'Position');
position(3) = 1200; position(4) = 700;
set(gcf,'Position',position);
subplot(1,2,1); hold on
plot(SToWls(S),T_targetQuantal2','k','LineWidth',3);
plot(SToWls(S),T_predictQuantalCIE2','r','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIE2Nomo','g','LineWidth',0.5);
plot(SToWls(S_cones_sp),T_cones_sp','b:','LineWidth',0.5);
title('S-S 2-deg fundamentals (blk), table constructed (red), nomo constructed (grn), Smith-Pokorny (blue dash)');
ylabel('Normalized quantal sensitivity');
xlabel('Wavelength');
subplot(1,2,2); hold on
plot(SToWls(S),log10(T_targetQuantal2'),'k','LineWidth',3);
plot(SToWls(S),log10(T_predictQuantalCIE2'),'r','LineWidth',1);
plot(SToWls(S),log10(T_predictQuantalCIE2Nomo'),'g','LineWidth',0.5);
plot(SToWls(S_cones_sp),log10(T_cones_sp'),'b:','LineWidth',0.5);
ylim([lowEndLogPlot 0.5]);
ylabel('Log10 normalized quantal sensitivity');
xlabel('Wavelength');
title('S-S 2-deg fundamentals (blk), table constructed (red), nomo constructed (grn), Smith-Pokorny (blue dash)');
drawnow;
if (DUMPFIGURES)
    if (exist('savefig','file'))
        savefig('Construct2DegreeCIE',gcf,'pdf');
    else
        saveas(gcf,'Construct2DegreeCIE','pdf');
    end
end

%% Compute 10 degree and plot
T_predictQuantalCIE10 = ComputeCIEConeFundamentals(S,10,32,3);
T_predictQuantalCIE10Nomo = ComputeCIEConeFundamentals(S,10,32,3,[558.9 530.3 420.7]');
figure; clf; hold on
position = get(gcf,'Position');
position(3) = 1200; position(4) = 700;
set(gcf,'Position',position);
subplot(1,2,1); hold on
plot(SToWls(S),T_targetQuantal10','k','LineWidth',3);
plot(SToWls(S),T_predictQuantalCIE10','r','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIE10Nomo','g','LineWidth',0.5);
title('S-S 10-deg fundamentals (blk), table constructed (red), nomo constructed (grn)');
ylabel('Normalized quantal sensitivity');
xlabel('Wavelength');
subplot(1,2,2); hold on
plot(SToWls(S),log10(T_targetQuantal10'),'k','LineWidth',3);
plot(SToWls(S),log10(T_predictQuantalCIE10'),'r','LineWidth',1);
plot(SToWls(S),log10(T_predictQuantalCIE10Nomo'),'g','LineWidth',0.5);
ylim([lowEndLogPlot 0.5]);
title('S-S 10-deg fundamentals (blk), table constructed (red), nomo constructed (grn)');
ylabel('Log10 normalized quantal sensitivity');
xlabel('Wavelength');
drawnow;
if (DUMPFIGURES)
    if (exist('savefig','file'))
        savefig('Construct10DegreeCIE',gcf,'pdf');
    else
        saveas(gcf,'Construct10DegreeCIE','pdf');
    end
end

%% Explore age
T_predictQuantalCIE20yrs = ComputeCIEConeFundamentals(S,2,20,3);
T_predictQuantalCIE59yrs = ComputeCIEConeFundamentals(S,2,59,3);
T_predictQuantalCIE75yrs = ComputeCIEConeFundamentals(S,2,75,3);
figure; clf; hold on
position = get(gcf,'Position');
position(3) = 1200; position(4) = 700;
set(gcf,'Position',position);
subplot(1,2,1); hold on
plot(SToWls(S),T_predictQuantalCIE2(1,:)','k','LineWidth',2);
plot(SToWls(S),T_predictQuantalCIE20yrs(1,:)','r','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIE59yrs(1,:)','g','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIE75yrs(1,:)','b','LineWidth',1);
ylabel('Normalized quantal sensitivity');
xlabel('Wavelength');
title('L cones, 32, 20, 59, 75 yo');
subplot(1,2,2); hold on
plot(SToWls(S),T_predictQuantalCIE2(3,:)','k','LineWidth',2);
plot(SToWls(S),T_predictQuantalCIE20yrs(3,:)','r','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIE59yrs(3,:)','g','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIE75yrs(3,:)','b','LineWidth',1);
ylabel('Normalized quantal sensitivity');
xlabel('Wavelength');
title('S cones, 32, 20, 59, 75 yo');
if (DUMPFIGURES)
    if (exist('savefig','file'))
        savefig('EffectOfAgeCIEFundamentals',gcf,'pdf');
    else
        saveas(gcf,'EffectOfAgeCIEFundamentals','pdf');
    end
end

%% Explore pupil size.  This effect, although it is listed in
% the CIE report, appears to be trivial.
T_predictQuantalCIE5mm = ComputeCIEConeFundamentals(S,2,32,5);
T_predictQuantalCIE7mm = ComputeCIEConeFundamentals(S,2,32,7);
figure; clf; hold on
position = get(gcf,'Position');
position(3) = 1200; position(4) = 700;
set(gcf,'Position',position);
subplot(1,2,1); hold on
plot(SToWls(S),T_predictQuantalCIE2(1,:)','k','LineWidth',2);
plot(SToWls(S),T_predictQuantalCIE5mm(1,:)','r','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIE7mm(1,:)','g','LineWidth',1);
ylabel('Normalized quantal sensitivity');
xlabel('Wavelength');
title('L cones, 3 mm, 5 mm, 7 mm');
subplot(1,2,2); hold on
plot(SToWls(S),T_predictQuantalCIE2(3,:)','k','LineWidth',2);
plot(SToWls(S),T_predictQuantalCIE5mm(3,:)','r','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIE7mm(3,:)','g','LineWidth',1);
ylabel('Normalized quantal sensitivity');
xlabel('Wavelength');
title('S cones, 3 mm, 5 mm, 7 mm');
if (DUMPFIGURES)
    if (exist('savefig','file'))
        savefig('EffectOfPupilCIEFundamentals',gcf,'pdf');
    else
        saveas(gcf,'EffectOfPupilCIEFundamentals','pdf');
    end
end

%% Explore varying lambdaMax
T_predictQuantalCIENominal = ComputeCIEConeFundamentals(S,2,32,3,[558.9 530.3 420.7]');
T_predictQuantalCIEShiftPlus = ComputeCIEConeFundamentals(S,2,32,3,[558.9 530.3 420.7]'+15);
T_predictQuantalCIEShiftMinus = ComputeCIEConeFundamentals(S,2,32,3,[558.9 530.3 420.7]'-15);
figure; clf; hold on
position = get(gcf,'Position');
position(3) = 1200; position(4) = 700;
set(gcf,'Position',position);
subplot(1,2,1); hold on
plot(SToWls(S),T_predictQuantalCIE2(1,:)','k','LineWidth',2);
plot(SToWls(S),T_predictQuantalCIENominal(1,:)','r','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIEShiftPlus(1,:)','g','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIEShiftMinus(1,:)','b','LineWidth',1);
ylabel('Normalized quantal sensitivity');
xlabel('Wavelength');
title('L cones, Nominal, +/- 15 nm lamba max');
subplot(1,2,2); hold on
plot(SToWls(S),T_predictQuantalCIE2(3,:)','k','LineWidth',2);
plot(SToWls(S),T_predictQuantalCIENominal(3,:)','r','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIEShiftPlus(3,:)','g','LineWidth',1);
plot(SToWls(S),T_predictQuantalCIEShiftMinus(3,:)','b','LineWidth',1);
ylabel('Normalized quantal sensitivity');
xlabel('Wavelength');
title('S cones, Nominal, +/- 15 nm lamba max');
if (DUMPFIGURES)
    if (exist('savefig','file'))
        savefig('EffectOfLambdaMaxCIEFundamentals',gcf,'pdf');
    else
        saveas(gcf,'EffectOfLambdaMaxCIEFundamentals','pdf');
    end
end

%% Generate a rod spectral sensitivity and compare with the CIE 1951
% rod spectral sensitivities
%
% The agreement will depend on rodLambdaMax, rodAxialDensity, and the
% nomogram used.  We are working on using some fitting to identify
% good values.
rodNomogram = 'StockmanSharpe';
rodLambdaMax = 490.3;
rodAxialDensity = 0.4;
targetRaw = load('T_rods');
T_targetEnergy = SplineCmf(targetRaw.S_rods,targetRaw.T_rods,S,2);
T_targetQuantalRods = QuantaToEnergy(S,T_targetEnergy')';
T_targetQuantalRods = T_targetQuantalRods/max(T_targetQuantalRods(:));
T_predictQuantalRods = ComputeCIEConeFundamentals(S,10,32,3,rodLambdaMax,rodNomogram,[],true,rodAxialDensity);
figure; clf; hold on
position = get(gcf,'Position');
position(3) = 600; position(4) = 700;
set(gcf,'Position',position);
plot(SToWls(S),T_targetQuantalRods','k','LineWidth',3);
plot(SToWls(S),T_predictQuantalRods','r','LineWidth',1);
title('Rod fundamentals (blk), constructed (red)');
ylabel('Normalized quantal sensitivity');
xlabel('Wavelength');

%% Test if the adjusted individual difference parameters get returned properly
indDiffParams.dlens = 0;
indDiffParams.dmac = 0;
indDiffParams.dphotopigment = [0 0 0];
indDiffParams.lambdaMaxShift = [];
[~,~,~,adjIndDiffParamsNoChange] = ComputeCIEConeFundamentals(S,10,32,5,[], [], [], false,[],[],indDiffParams);

% Change all parameters by some amount
indDiffParamsNew.dlens = 2;
indDiffParamsNew.dmac = -5;
indDiffParamsNew.dphotopigment = [5 3 -4];
indDiffParamsNew.lambdaMaxShift = [];
[~,~,~,adjIndDiffParamsNew] = ComputeCIEConeFundamentals(S,10,32,5,[], [], [], false,[],[],indDiffParamsNew);

% Calculate if they are in agreement
fprintf('* Checking for individual difference adjustment\n');
fprintf('> Lens density\n');
fprintf('\tInput: \t%.2f%s\n', indDiffParamsNew.dlens, '%');
fprintf('\tOutput: %.2f%s\n', (max(log10(adjIndDiffParamsNew.lens) ./ log10(adjIndDiffParamsNoChange.lens))-1)*100, '%');

fprintf('> Macular pigment density\n');
fprintf('\tInput: \t%.2f%s\n', indDiffParamsNew.dmac, '%');
fprintf('\tOutput: %.2f%s\n', (max(log10(adjIndDiffParamsNew.mac) ./ log10(adjIndDiffParamsNoChange.mac))-1)*100, '%');

fprintf('> L photopigment density\n');
fprintf('\tInput: \t%.2f%s\n', indDiffParamsNew.dphotopigment(1), '%');
fprintf('\tOutput: %.2f%s\n', ((adjIndDiffParamsNew.dphotopigment(1)/adjIndDiffParamsNoChange.dphotopigment(1))-1)*100, '%');

fprintf('> M photopigment density\n');
fprintf('\tInput: \t%.2f%s\n', indDiffParamsNew.dphotopigment(2), '%');
fprintf('\tOutput: %.2f%s\n', ((adjIndDiffParamsNew.dphotopigment(2)/adjIndDiffParamsNoChange.dphotopigment(2))-1)*100, '%');

fprintf('> S photopigment density\n');
fprintf('\tInput: \t%.2f%s\n', indDiffParamsNew.dphotopigment(3), '%');
fprintf('\tOutput: %.2f%s\n', ((adjIndDiffParamsNew.dphotopigment(3)/adjIndDiffParamsNoChange.dphotopigment(3))-1)*100, '%');
