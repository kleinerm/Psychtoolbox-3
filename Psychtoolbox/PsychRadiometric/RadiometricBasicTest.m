% RadiometricBasicTest
%
% Some basic tests of our radiometric calculations
%
% 2/27/13  dhb  Wrote it.

%% Clear and close
clear; close all

%% See if we can match some conversions computed
% by Ed Pugh and conveyed to me by Brian Wandell.
%
% Ed starts with retinal illuminance of 10^15 in quanta/[cm2-sec]'
%   He gets 340 uW/cm2.  We get 342.
%   He gets ~590,000 trolands.  We get about this assuming 17 mm eye length.
%   He gets ~190,000 cd/m2 for a 2mm diameter pupil.  We also get about this.
retinalIlluminanceQuantaPerCm2Sec = 1e15;
wavelengthNm = 580;
pupilDiamMm = 2;
retinalIlluminanceWattsPerCm2 = QuantaToEnergy(wavelengthNm,retinalIlluminanceQuantaPerCm2Sec);
retinalIlluminanceUWattsPerCm2 = 1e6*retinalIlluminanceWattsPerCm2;
retinalIlluminanceWattsPerUm2 = 1e-8*retinalIlluminanceWattsPerCm2;
fprintf('Retinal illuminance of %0.1f log10 quanta/[cm2-sec]\n',log10(retinalIlluminanceQuantaPerCm2Sec));
fprintf('\tconverts to %0.1f log10 watts/[cm2-sec] (%0.1f uWatts/cm2)\n',log10(retinalIlluminanceWattsPerCm2),...
    retinalIlluminanceUWattsPerCm2);
photopicTrolands = RetIrradianceToTrolands(retinalIlluminanceWattsPerUm2, wavelengthNm, 'Photopic','Human','17');
fprintf('\tconverts to %0.1f photopic trolands\n',photopicTrolands);
photopicLuminanceCdM2 = TrolandsToLum(photopicTrolands,(pi/4)*pupilDiamMm^2);
fprintf('\tconverts to %0.1f cd/m2 (%d mm pupil)\n',photopicLuminanceCdM2,pupilDiamMm);

%% Check ANSI light limit calculations against numbers in Eds document.  He doesn't say
% the durations or size he assumed, but he does say he got the numbers from Delori's
% spreadsheet.  Let's try making up a source size and duration and see what happens.
stimulusSizeDeg = 2;
stimulusDurationSec = 1;


% When I plug these numbers (580 nm, 2 degree stimulus, 2 mm pupil diameter, 1 second exposure)
% into the version of Delori's spreadsheet I got via Ed Pugh (rev 1/10/08), it computes that
% the stimulus has a radiance of 31.28 uW/[cm2 sr], a corneal irradiance of 29.93 uW/cm2,
% total radiant power in the pupil of 940.33 nW, and a retinal irradiance of 340 uW/cm2.
%
% It computes the exposure safety limit for this stimulus as 2.96 mW of radiant power in
% the pupil and a retinal illuminance 1.07 W/cm2.  Radiant power limit in the pupil is given
% as 2.96 mJ and radiant exposure on  the retina as 1.07 J/cm2

% Now compute the same quantities with PTB routines.  First the stimulus.  From above
% we already have the retinal irradiance at about 340 Watts/cm2.  We get 342.5. I think
% the spreadsheet rounds down to 340.

[~, MPELimitRadiance_WattsPerCm2Sr2,MPELimitCornealIrradiance_WattsPerCm2] = ...
    AnsiZ136MPEComputeExtendedSourceLimit(stimulusDurationSec,stimulusSizeDeg,wavelengthNm);
MPELimitRetinalIlluminanceWattsPerUm2 = RadianceToRetIrradiance(1e-4*MPELimitRadiance_WattsPerCm2Sr2,wavelengthNm,(pi/4)*pupilDiamMm^2,17);
MPELimitRetinalIlluminanceUWattsPerCm2 = 1e6*1e8*MPELimitRetinalIlluminanceWattsPerUm2;
fprintf('MPE limit computed as %g uWatts/cm2\n',MPELimitRetinalIlluminanceUWattsPerCm2);
.



