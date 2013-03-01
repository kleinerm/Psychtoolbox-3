function radianceWattsPerM2Sr = RetIrradianceToRadiance(irradianceWattsPerUm2,irradianceS,pupilAreaMM,eyeSizeMM)
% radianceWattsPerM2Sr = RetIrradianceToRadiance(irradianceWattsPerUm2,irradianceS,pupilAreaMM,eyeSizeMM)
%
% Perform the geometric calculations necessary to convert a measurement of retinal
% irradiance to the source radiance that would produce it.
%
%   Input irradianceWattsPerUm2 is in units of power/um^2-wlinterval.
%   Input irradianceS gives the wavelength sampling information.
%   Input pupilAreaMM should be in units of mm^2.
%   Input eyeSizeMM should be the length of the eye in mm.
%   Output radianceWattsPerM2Sr is in units of power/m^2-sr-wlinterval.
%
%   Light power may be expressed in watts or quanta-sec or in your
%   favorite units.  Indeed, it may also be passed as energy rather
%   than power.  
%
% This conversion does not take absorption in the eye into account,
% as this is more conveniently foldeded into the spectral absorbtance.
%
% See also: PupilAreaFromLum, EyeLength, RadianceToRetIrradiance.
%
% 2/28/13  dhb  Wrote it.

% Convert units from um^2 to mm^2 base.
irradianceWattsPerMm2 = irradianceWattsPerUm2*1e6;

% Define factor to convert radiance spectrum to retinal irradiance in watts/mm^2-wlinterval.
% We then apply this in the opposite direction.  See RadianceToRetIrradiance for the
% conversion logic.
conversionFactor = pupilAreaMM/(eyeSizeMM^2);
radianceWattsPerMm2Sr = irradianceWattsPerMm2/conversionFactor;

% Convert radiance to power/sr-m^2-wlinterval
radianceWattsPerM2Sr = radianceWattsPerMm2Sr*1e6;


