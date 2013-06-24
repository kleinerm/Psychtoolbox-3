function retIrradiance_PerDegrees2 = radRetIrradiancePerAreaAndEyeLengthToRetIrradiancePerDegrees2(retIrradiance_PerArea,eyeLength)
% retIrradiance_PerDegrees2 = radRetIrradiancePerAreaAndEyeLengthToRetIrradiancePerDegrees2(retIrradiance_PerArea,eyeLength)
%
% Convert retinal irradiance measured in units of Y/x^2 to units of
% Y/deg^2, where x is a unit of distance (m, cm, mm, um, etc.) and
% Y is a measure of light amount (Watts, Joules, quanta/sec, quanta, etc.)
%
% Eye length should be passed in units of x.
%
% See also: PsychRadiometric, radRetIrradiancePerDegrees2AndEyeLengthToRetIrradiancePerArea.
%
% 6/23/13  dhb  Wrote it.

% Convert x to degrees.  The routine RetinalMMToDegrees does not
% actually care whether the input is in mm, it just needs its
% two arguments to be in the same units.
degreesPerX = RetinalMMToDegrees(1,eyeLength);
degrees2PerArea = degreesPerX^2;
retIrradiance_PerDegrees2 = retIrradiance_PerArea/degrees2PerArea;

