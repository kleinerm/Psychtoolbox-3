% RadiometricBasicTest
%
% Some basic tests of our radiometric calculations and light
% safety calculations.
%
% These read a series of monochromatic specifications from
% a tab delimited text file and compute various quantities
% from them.
%
% 2/27/13  dhb  Wrote it.

%% Clear and close
clear; close all

%% Load conditions
conditionStructs = ReadStructsFromText('RadiometricBasicTestInput.txt');

%% Loop over conditions and report
for i = 1:length(conditionStructs)
    % Set variables for this condition
    retinalIlluminanceUWattsPerCm2 = conditionStructs(i).retinalIlluminanceUWattsPerCm2;
    wavelengthNm = conditionStructs(i).wavelengthNm;
    stimulusDiameterDegrees = conditionStructs(i).stimulusDiamDegrees;
    stimulusDurationSeconds = conditionStructs(i).stimulusDurationSecs;
    eyeLengthMm = conditionStructs(i).eyeLengthMm;
    pupilDiameterMm = conditionStructs(i).pupilDiameterMm;
    ansiEyeLengthMm = conditionStructs(i).ansiEyeLengthMm;
    ansiPupilDiameterMm = conditionStructs(i).ansiPupilDiameterMm;   
    fprintf('**********\nCondition %d\n\tInput retinal illuminance of %0.1f uWatts/cm2\n',i,retinalIlluminanceUWattsPerCm2);
    fprintf('\t\tWavelength %d nm\n',wavelengthNm);
    fprintf('\t\tStimulus diamter %0.1f degrees\n',stimulusDiameterDegrees);
    fprintf('\t\tStimulus duration %0.1f seconds\n',stimulusDurationSeconds);
    fprintf('\t\tEye length %0.1f mm\n',eyeLengthMm);
    fprintf('\t\tPupil diameter %0.1f mm\n',pupilDiameterMm);
    fprintf('\t\tAssuming ANSI standard eye length of %0.1f mm\n',ansiEyeLengthMm);
    fprintf('\t\tAssuming ANSI pupil diameter of %0.1f mm\n',ansiPupilDiameterMm);
    
    % Get comparison values from Delori spreadsheet.  These need to be computed
    % by hand using the spreadsheet and then entered into the condition file.
    deloriRadianceMWattsPerCm2Sr = conditionStructs(i).deloriRadianceMWattsPerCm2Sr;
    deloriCornealIrradianceUWattsPerCm2 = conditionStructs(i).deloriCornealIrradianceUWattsPerCm2;
    deloriPowerInPupilMWatts = conditionStructs(i).deloriPowerInPupilMWatts;
    deloriLog10PhotopicTrolands = conditionStructs(i).deloriLog10PhotopicTrolands;
    deloriLog10ScotopicTrolands = conditionStructs(i).deloriLog10ScotopicTrolands;
    deloriMPERetinalIrradianceWattsPerCm2 = conditionStructs(i).deloriMPERetinalIrradianceWattsPerCm2;
    deloriMPEPowerInPupilMWatts = conditionStructs(i).deloriMPEPowerInPupilMWatts;
    
    % In some cases we have other comparison values for other sources
    checkPhotopicLuminanceCdM2 = conditionStructs(i).checkPhotopicLuminanceCdM2;
    checkRetinalIlluminanceQuantaPerCm2Sec = conditionStructs(i).checkRetinalIlluminanceQuantaPerCm2Sec;
    
    % Do unit conversions and print with comparisons when such are available
    retinalIlluminanceWattsPerCm2 = 1e-6*retinalIlluminanceUWattsPerCm2;
    retinalIlluminanceWattsPerUm2 = 1e-8*retinalIlluminanceWattsPerCm2;
    retinalIlluminanceQuantaPerCm2Sec = EnergyToQuanta(wavelengthNm,retinalIlluminanceWattsPerCm2);
    photopicTrolands = RetIrradianceToTrolands(retinalIlluminanceWattsPerUm2, wavelengthNm, 'Photopic','Human',num2str(eyeLengthMm));
    scotopicTrolands = RetIrradianceToTrolands(retinalIlluminanceWattsPerUm2, wavelengthNm, 'Scotopic','Human',num2str(eyeLengthMm));
    photopicLuminanceCdM2 = TrolandsToLum(photopicTrolands,(pi/4)*pupilDiameterMm^2);
    radianceWattsPerM2Sr = RetIrradianceToRadiance(retinalIlluminanceWattsPerUm2,wavelengthNm,(pi/4)*pupilDiameterMm^2,eyeLengthMm);
    radianceMWattsPerCm2Sr = 1e3*1e-4*radianceWattsPerM2Sr;
    cornealIrradianceMWattsPerCm2 = RadianceAndDegrees2ToCornealIrradiance(radianceMWattsPerCm2Sr,(pi/4)*stimulusDiameterDegrees^2);
    cornealIrradianceUWattsPerCm2 = 1e3*cornealIrradianceMWattsPerCm2;
    powerInPupilUWatts = cornealIrradianceUWattsPerCm2*(1e-2)*(pi/4)*pupilDiameterMm^2;
    powerInPupilMWatts = 1e-3*powerInPupilUWatts;
    pupilEnergyMJoules = powerInPupilMWatts*stimulusDurationSeconds;
    
    fprintf('\tConverts to retinal illuminance of %0.1f log10 quanta/[cm2-sec] (cf. %0.1f)\n',log10(retinalIlluminanceQuantaPerCm2Sec),log10(checkRetinalIlluminanceQuantaPerCm2Sec));
    fprintf('\tConverts to %0.2f log10 photopic trolands (cf. %0.2f)\n',log10(photopicTrolands),deloriLog10PhotopicTrolands);
    fprintf('\tConverts to %0.2f log10 scotopic trolands (cf. %0.2f)\n',log10(scotopicTrolands),deloriLog10ScotopicTrolands);
    fprintf('\tConverts to %0.1f cd/m2 (cf. %0.1f)\n',photopicLuminanceCdM2,checkPhotopicLuminanceCdM2);
    fprintf('\tConverts to radiance %0.1f mWatts/[cm2-sr] (cf. %0.1f)\n',radianceMWattsPerCm2Sr,deloriRadianceMWattsPerCm2Sr);
    fprintf('\tConverts to corneal irradiance %0.1f uWatts/cm2 (cf. %0.1f)\n',cornealIrradianceUWattsPerCm2,deloriCornealIrradianceUWattsPerCm2);
    fprintf('\tConverts to total radiant power in the pupil of %0.2g mW (cf. %0.2g)\n',powerInPupilMWatts,deloriPowerInPupilMWatts);
    
    % Compute MPE, with comparisons
    [MPELimitIntegratedRadiance_JoulesPerCm2Sr, ...
        MPELimitRadiance_WattsPerCm2Sr, ...
        MPELimitCornealIrradiance_WattsPerCm2, ...
        MPELimitCornealRadiantExposure_JoulesPerCm2] = ...
        AnsiZ136MPEComputeExtendedSourceLimit(stimulusDurationSeconds,stimulusDiameterDegrees,wavelengthNm);
    MPELimitRadiance_WattsPerM2Sr =  1e4*MPELimitRadiance_WattsPerCm2Sr;
    MPELimitRetinalIlluminanceWattsPerUm2 = RadianceToRetIrradiance(MPELimitRadiance_WattsPerM2Sr,wavelengthNm,(pi/4)*ansiPupilDiameterMm^2,ansiEyeLengthMm);
    MPELimitRetinalIlluminanceWattsPerCm2 = 1e8*MPELimitRetinalIlluminanceWattsPerUm2;
    MPELimitRetinalIlluminanceUWattsPerCm2 = 1e6*MPELimitRetinalIlluminanceWattsPerCm2;
    MPELimitPowerInPupilWatts = MPELimitCornealIrradiance_WattsPerCm2*(pi/4)*((1e-1*ansiPupilDiameterMm)^2);
    MPELimitPowerInPupilMWatts = 1e3*MPELimitPowerInPupilWatts;
    fprintf('\tMPE retinal illuminance limit computed as %0.3g Watts/cm2 (cf. %0.3g)\n',MPELimitRetinalIlluminanceWattsPerCm2,deloriMPERetinalIrradianceWattsPerCm2);
    fprintf('\t\tLimit - Stimulus log10 difference: %0.1f log10 units\n',log10(MPELimitRetinalIlluminanceWattsPerCm2)-log10(retinalIlluminanceWattsPerCm2));
    fprintf('\tMPE power in pupil limit %0.3g mWatts (cf. %0.3g)\n',MPELimitPowerInPupilMWatts,deloriMPEPowerInPupilMWatts);
    fprintf('\t\tLimit - Stimulus log10 difference: %0.1f log10 units\n',log10(MPELimitPowerInPupilMWatts)-log10(powerInPupilMWatts));
    fprintf('\n');

end

%% See if we can match some conversions computed
% by Ed Pugh and conveyed to me by Brian Wandell.
%
% Ed starts with retinal illuminance of 10^15 in quanta/[cm2-sec]'
%   He gets 340 uW/cm2.  We get 342.
%   He gets ~590,000 trolands.  We get about this assuming 17 mm eye length.
%   He gets ~190,000 cd/m2 for a 2mm diameter pupil.  We also get about this.




%% Check ANSI light limit calculations against numbers in Eds document.  He doesn't say
% the durations or size he assumed, but he does say he got the numbers from Delori's
% spreadsheet.  Let's try making up a source size and duration and see what happens.


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

% The spreadsheet computes the exposure safety limit for this stimulus as:
%  radiant power in the pupil 2.96 mW 
%  radiant energy in the pupil of 2.96 mJ
%  retinal irradiance 1.07 W/cm2
%  retinal exposure of 1.07 J/cm2





