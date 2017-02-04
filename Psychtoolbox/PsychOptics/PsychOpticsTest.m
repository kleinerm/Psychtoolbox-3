% PsychOpticsTest
%
% Some basic tests and comparisons of PsychOptics routines, in particular our ability
% to go back and forth between LSF's and PSF's.

%% Clear
clear; close all; 

%% Compute Westheimer and Davila/Geisler LSFs from the formulae provided.
%
% Set up spatial sampling in one-dimension, both in space domain and
% corresponding frequency domain.
% For the symmetric phase component to work nSamples MUST BE EVEN and the
% 1D LSF support must be long enough so that the LSF goes to zero

% Define spatial support.
%
% Number of samples must be even.
nSpatialSamples = 512;
centerPosition = nSpatialSamples/2+1;
maxPositionMinutes = 15;
integerSamples1D = -nSpatialSamples/2:nSpatialSamples/2-1;
distanceMinutes1D = maxPositionMinutes*integerSamples1D/(centerPosition-1);

% These produce two similar lsf's for the human eye, from the literature.
WestLSF = WestLSFMinutes(abs(distanceMinutes1D));
DavilaGeislerLSF = DavilaGeislerLSFMinutes(abs(distanceMinutes1D));

% Westheimer also gives a formula for the PSF (in addition to the LSF).
%
% Get this for comparison.
distanceMinutes2D = maxPositionMinutes*MakeRadiusMat(nSpatialSamples,nSpatialSamples,centerPosition)/(centerPosition-1);
if (any(distanceMinutes2D(centerPosition,:) ~= abs(distanceMinutes1D)))
    error('Did not build correct 2D radius matrix.');
end
WestPSFFormula = WestPSFMinutes(abs(distanceMinutes2D));
WestPSFFormula = WestPSFFormula/sum(WestPSFFormula(:));
if (WestPSFFormula(centerPosition,centerPosition) ~= max(WestPSFFormula(:)))
    error('We don''t understand spatial coordinates as well as we should.');
end

%% Get PSFs from LSF
WestPSFDerived = LsfToPsf(WestLSF);
DavilaGeislerPSFDerived = LsfToPsf(DavilaGeislerLSF);

%% And get LSF back again.
%
% This is done by convolution and if things are working will produce
% what we started with to good approximation.
WestLSFFromPSFFormula = PsfToLsf(WestPSFFormula);
WestLSFFromPSFDerived = PsfToLsf(WestPSFDerived);
DavilaGeislerLSFFromPSFDerived = PsfToLsf(DavilaGeislerPSFDerived);

%% Check that max of returned psf is where we think it should be.
%
% This check does assume an PSF with its max at 0, which is true for the
% Westheimer and Davila-Geisler cases.
if (WestPSFDerived(centerPosition,centerPosition) ~= max(WestPSFDerived(:)))
    error('We don''t understand spatial coordinates as well as we should.');
end
if (DavilaGeislerPSFDerived(centerPosition,centerPosition) ~= max(DavilaGeislerPSFDerived(:)))
    error('We don''t understand spatial coordinates as well as we should.');
end

%% Make a figure that compares the original and derived LSFs
figure;
set(gcf,'Position',[100 100 1200 800]);
set(gca, 'FontSize', 14);
subplot(2,2,1); hold on
plot(distanceMinutes1D,WestLSF,'r','LineWidth',4);
plot(distanceMinutes1D,WestLSFFromPSFDerived,'g-', 'LineWidth', 2);
plot(distanceMinutes1D,WestLSFFromPSFFormula,'k-', 'LineWidth', 2);
xlim([-4 4]);
xlabel('Position (minutes');
ylabel('Normalized LSF');
title('Westheimer')
legend({'Original','Recovered from Derived PSF','Recovered from Formula PSF'},'Location','NorthEast');

subplot(2,2,2); hold on
plot(distanceMinutes1D,DavilaGeislerLSF,'r','LineWidth',4);
plot(distanceMinutes1D,DavilaGeislerLSFFromPSFDerived,'g-', 'LineWidth', 2);
xlim([-4 4]);
xlabel('Position (minutes');
ylabel('Normalized LSF');
title('Davila-Geisler');
legend({'Original','Recovered from PSF'},'Location','NorthEast');

subplot(2,2,3); hold on
plot(distanceMinutes1D,WestPSFDerived(centerPosition,:)/max(WestPSFDerived(centerPosition,:)),'r','LineWidth',4);
plot(distanceMinutes1D,WestPSFFormula(centerPosition,:)/max(WestPSFFormula(centerPosition,:)),'k-','LineWidth',2);
xlim([-4 4]);
title('Westheimer')
xlabel('Position (minutes');
ylabel('Normalized PSF Slice');
legend({'Derived from LSF','Formula PSF'},'Location','NorthEast');

subplot(2,2,4); hold on
plot(distanceMinutes1D,DavilaGeislerPSFDerived(centerPosition,:)/max(DavilaGeislerPSFDerived(centerPosition,:)),'r','LineWidth',4);
xlim([-4 4]);
xlabel('Position (minutes');
ylabel('Normalized PSF Slice');
title('Davila-Geisler')
legend({'Derived from LSF'},'Location','NorthEast');





