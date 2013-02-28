% Psychtoolbox:PsychRadiometric.
%
% Radiometric and photometric calculations.  See also the closely related
% Psychtoolbox:PsychColorimetric and its related data folders.  Sometimes
% it is not entirely clear whether a routine is better classified as
% radiometric or colorimetric. Apologies if our intuitions don't match yours
%
% help Psychtoolbox              % For an overview, triple-click me & hit enter.
% help PsychColorimetric         % For colorimetric calculations, triple-click me & hit enter.
% help PsychColorimetricData     % For colorimetric calculations, triple-click me & hit enter.
% help PsychColorimetricMatFiles % For list of data .mat files, triple-click me & hit enter.
%
%   CornealIrradianceAndDegrees2ToRadiance - Convert corneal irradiance to radiance, given stimulus area in degrees^2.
%   EnergyToQuanta      - Convert monochromatic energy to quanta.
%   PowerToTrolands     - Convert monochromatic power to photopic trolands.
%   PsychAnsiZ136MPE    - Ansi 136.1-2007 standard for maximum permissible light exposure.
%   QuantaToEnergy      - Convert monochromatic quanta to energy.
%   QuantaToTrolands    - Convert monochromatic quanta to photopic trolands.
%   RadianceAndDegrees2ToCornealIrradiance - Convert radiance to corneal irradiance, given stimulus area in degrees^2.
%   RadianceAndDistanceAreaToCornealIrradiance - Convert radiance to corneal irradiance, given stimulus area and distance in linear units (e.g, cm).
%   RadianceToRetIrradiance - Convert spectral radiance to spectral retinal irradiance (power units).
%   RadiometricBasicTest - Some basic tests of radiometric conversions.
%   RetIrradianceToTrolands - Convert retinal irradiance (power units) to trolands.
%   TrolandsToLum       - Convert trolands to luminance (cd/m2).
%   TrolandsToPower     - Convert monochromatic photopic trolands to power.
%   TrolandsToQuanta    - Convert monochromatic photopic trolands to quanta.
%   TrolandsToRetIrradiance - Get retinal irradiance (power units) from spectrum and trolands.
%   WattsToRetIrradiance - Get absolute retinal irradiance (power units) from rel. spectrum and watts/area.
  
% Copyright (c) 1996-2013 by David Brainard, Denis Pelli, & Mario Kleiner



