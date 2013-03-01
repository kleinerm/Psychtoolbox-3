function irradianceWattsPerUm2 = RadianceToRetIrradiance(radianceWattsPerM2Sr,radianceS,pupilAreaMM,eyeSizeMM)
% irradianceWattsPerUm2 = RadianceToRetIrradiance(radianceWattsPerM2Sr,radianceS,pupilAreaMM,eyeSizeMM)
%
% Perform the geometric calculations necessary to convert a measurement of source
% radiance to corresponding retinal irradiance. 
%
%   Input radianceWattsPerM2Sr should be in units of power/m^2-sr-wlinterval.
%   Input radianceS gives the wavelength sampling information.
%   Input pupilAreaMM should be in units of mm^2.
%   Input eyeSizeMM should be the length of the eye in mm.
%   Output irradianceWattsPerUm2 is in units of power/um^2-wlinterval.
%
%   Light power may be expressed in watts or quanta-sec or in your
%   favorite units.  Indeed, it may also be passed as energy rather
%   than power.  
%
% This conversion does not take absorption in the eye into account,
% as this is more conveniently foldeded into the spectral absorbtance.
%
% See also: PupilAreaFromLum, EyeLength, RetIrradianceToRadiance.
%
% 7/10/03  dhb  Wrote it.
% 11/06/03 dhb  Fixed comments about units, as per Lu Yin email.
% 3/29/12  dhb  Comment on output units was wrong.  It said power/um^2-sec-wlinterval
%               but the 'sec' part makes no sense given the 'power' in the numerator.
% 2/28/13  dhb  Make units clear in variable names.

% Convert spectral units to power/sr-mm^2-wlinterval
radianceWattsPerMm2Sr = radianceWattsPerM2Sr*1e-6;

% Define factor to convert radiance spectrum to retinal irradiance in watts/mm^2-wlinterval.
% Commented out code shows the logic, which is short circuited by actual code.
% but is conceptually convenient for doing the calculation.
%  distanceToSourceMM = 100;
%  fractionfSphere = pupilAreaMM/4*pi*distanceToSourceMM^2;
%  pupilAreaSR = 4*pi*fractionOfSphere;
%  sourceAreaMM = (distanceToSourceMM^2)/(eyeSizeMM^2);
%  conversionFactor = pupilAreaSR*sourceAreaMM;
conversionFactor = pupilAreaMM/(eyeSizeMM^2);
irradianceWattsPerMm2 = conversionFactor*radianceWattsPerMm2Sr;

% Convert units to um^2 from mm^2 base.
irradianceWattsPerUm2 = irradianceWattsPerMm2*1e-6;
