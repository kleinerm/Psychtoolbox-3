% RadiometricConversionsTest
%
% Test out the radiometric conversion routines.
%
% This is not a complete test.  And, I thought I wrote such a thing
% once before but can't currently find it.
%
% 6/23/13  dhb  Wrote it.
% 12/2/15  dhb  More tests.

%% Clear
clear; close all;

%% Define a monochromatic radiance
S = [500 1 1];
radianceWattsPerSrM2 = 2.7*ones(S(3),1);

%% Define eye parameters
eyeLengthM = 0.017;
pupilDiameterM = 0.003;
pupilAreaM2 = 2*pi*((pupilDiameterM/2)^2);

%% Define stimulus parameters
stimulusDistanceM = 4.2;
stimulusAreaM2 = 0.24;
stimulusSideDeg = (180/pi)*2*atan(sqrt(stimulusAreaM2)/(2*stimulusDistanceM));
stimulusAreaDeg2 = stimulusSideDeg^2;

%% Convert radiance to retinal irradiance
retIrradianceWattsPerM2 = RadianceAndPupilAreaEyeLengthToRetIrradiance(radianceWattsPerSrM2,S,pupilAreaM2,eyeLengthM);

%% Convert retinal irradiance to units of degrees2.
retIrradianceWattsPerDegrees2 = RetIrradiancePerAreaAndEyeLengthToRetIrradiancePerDegrees2(retIrradianceWattsPerM2,eyeLengthM);

%% Convert back to units of area and check that we get what we put in.
retIrradianceWattsPerM2Check = RetIrradiancePerDegrees2AndEyeLengthToRetIrradiancePerArea(retIrradianceWattsPerDegrees2,eyeLengthM);
fprintf('Retinal irradiance: %0.3g Watts/M2 (%0.3g check), %0.3g Watts/deg2\n', ...
    retIrradianceWattsPerM2,retIrradianceWattsPerM2Check,retIrradianceWattsPerDegrees2);
if (abs(retIrradianceWattsPerM2-retIrradianceWattsPerM2Check) > 1e-10)
    error('Retinal irradiance unit calculations do not invert');
else
    
%% Convert retinal irradiance to corneal irradiance and back
fractionalTolerance = 0.005;
cornIrradianceWattsPerM2 = RadianceAndDistanceAreaToCornIrradiance(radianceWattsPerSrM2,stimulusDistanceM,stimulusAreaM2);
cornIrradianceWattsPerM2Check = RadianceAndDegrees2ToCornIrradiance(radianceWattsPerSrM2,stimulusAreaDeg2);
fprintf('Corneal irradiance: %0.3g Watts/M2 (%0.3g check)\n', ...
    cornIrradianceWattsPerM2,cornIrradianceWattsPerM2Check);
if (abs(cornIrradianceWattsPerM2-cornIrradianceWattsPerM2Check)/cornIrradianceWattsPerM2 > fractionalTolerance)
    error('Two ways of computing corneal irradiance from radiance do not agree');
end
radianceWattsPerSrM2Check = CornIrradianceAndDegrees2ToRadiance(cornIrradianceWattsPerM2,stimulusAreaDeg2);
fprintf('Radiance: %0.4f Watts/[sr-M2] (%0.4f check)\n', ...
    radianceWattsPerSrM2,radianceWattsPerSrM2Check);
if (abs(radianceWattsPerSrM2-radianceWattsPerSrM2Check)/radianceWattsPerSrM2 > fractionalTolerance)
    error('Radiance to corneal irradiance and back does not agree');
end


end


