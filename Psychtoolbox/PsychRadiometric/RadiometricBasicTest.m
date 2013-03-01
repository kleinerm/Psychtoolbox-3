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
eyeLengthMm = 17;
retinalIlluminanceWattsPerCm2 = QuantaToEnergy(wavelengthNm,retinalIlluminanceQuantaPerCm2Sec);
retinalIlluminanceUWattsPerCm2 = 1e6*retinalIlluminanceWattsPerCm2;
retinalIlluminanceWattsPerUm2 = 1e-8*retinalIlluminanceWattsPerCm2;
fprintf('Retinal illuminance of %0.1f log10 quanta/[cm2-sec]\n',log10(retinalIlluminanceQuantaPerCm2Sec));
fprintf('\tconverts to %0.1f log10 watts/cm2 (%0.1f uWatts/cm2)\n',log10(retinalIlluminanceWattsPerCm2),...
    retinalIlluminanceUWattsPerCm2);
photopicTrolands = RetIrradianceToTrolands(retinalIlluminanceWattsPerUm2, wavelengthNm, 'Photopic','Human',num2str(eyeLengthMm));
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
% the stimulus has:
%   a retinal irradiance of 340 uW/cm2 
%   a radiance of 31.28 mW/[cm2 sr]
%   a corneal irradiance of 29.93 uW/cm2,
%   total radiant energy in the pupil of 940.33 nJ.
%   retinal radiant exposure of 340 uJ/cm2.
% We compute these quantities using PTB routines.  From above we already have the retinal irradiance
% at about 340 Watts/cm2. The rest of the numbers also match up pretty well.  Rounding in the spreadsheet
% plus perhaps a different assumption about eye length can explain the differences, I think.
radianceWattsPerM2Sr = RetIrradianceToRadiance(retinalIlluminanceWattsPerUm2,wavelengthNm,(pi/4)*pupilDiamMm^2,eyeLengthMm);
radianceMWattsPerCm2Sr = 1e3*1e-4*radianceWattsPerM2Sr;
fprintf('\tConverts to radiance %0.1f mWatts/[cm2-sr]\n',radianceMWattsPerCm2Sr);
cornealIrradianceMWattsPerCm2 = RadianceAndDegrees2ToCornealIrradiance(radianceMWattsPerCm2Sr,(pi/4)*stimulusSizeDeg^2);
cornealIrradianceUWattsPerCm2 = 1e3*cornealIrradianceMWattsPerCm2;
fprintf('\tConverts to corneal irradiance %0.1f uWatts/cm2\n',cornealIrradianceUWattsPerCm2);
pupilPowerUWatts = cornealIrradianceUWattsPerCm2*(1e-2)*(pi/4)*pupilDiamMm^2;
pupilPowerNWatts = 1e3*pupilPowerUWatts;
pupilEnergyNJoules = pupilPowerNWatts*stimulusDurationSec;
fprintf('\tConverts to total radiant energy in the pupil of %0.1f nJ\n',pupilEnergyNJoules);

% The spreadsheet computes the exposure safety limit for this stimulus as:
%  radiant power in the pupil 2.96 mW 
%  radiant energy in the pupil of 2.96 mJ
%  retinal irradiance 1.07 W/cm2
%  retinal exposure of 1.07 J/cm2
[MPELimitIntegratedRadiance_JoulesPerCm2Sr, ...
    MPELimitRadiance_WattsPerCm2Sr, ...
    MPELimitCornealIrradiance_WattsPerCm2, ...
    MPELimitCornealRadiantExposure_JoulesPerCm2] = ...
    AnsiZ136MPEComputeExtendedSourceLimit(stimulusDurationSec,stimulusSizeDeg,wavelengthNm);
 MPELimitRadiance_WattsPerM2Sr =  1e4*MPELimitRadiance_WattsPerCm2Sr;
MPELimitRetinalIlluminanceWattsPerUm2 = RadianceToRetIrradiance(MPELimitRadiance_WattsPerM2Sr,wavelengthNm,(pi/4)*pupilDiamMm^2,eyeLengthMm);
MPELimitRetinalIlluminanceWattsPerCm2 = 1e8*MPELimitRetinalIlluminanceWattsPerUm2
fprintf('MPE retial illuminance limit computed as %g Watts/cm2\n',MPELimitRetinalIlluminanceWattsPerCm2);




