function rc = kPsychNeedHDRWindow
% kPsychNeedHDRWindow -- Created onscreen window shall be a HDR window.
%
% This flag can be passed to the optional 'imagingmode' parameter of
% Screen('OpenWindow', ...) or PsychImaging('OpenWindow', ...).
%
% It will cause the onscreen window to be marked as using a HDR display
% mode. That means the windows display surface displays on a High dynamic
% range display device, color values are floating point values, and the
% values can and will lie outside the conventional unsigned normalized
% unorm value range of 0.0 - 1.0. Colors are expressed in a unit suitable
% for HDR display, e.g., multiples of 80 nits (defined as SDR intensity
% range) or in nits. E.g., a color value of 1234 could mean a light
% intensity of 1234 nits (candela per square-meter).
%
% This will change a couple of default settings wrt. image precision, and
% also map color values into the target HDR range of the window:
%
% 1. Screen('MakeTexture') will create textures of 16 bpc half-float format
% (aka fp16) by default if the 'floatprecision' flag is omitted, instead of
% creating 8 bpc fixed point textures by default. This is because 8 bpc fixed
% point with only 256 intensity levels is usually inadequate for precision
% image display on a HDR display, and color values outside the unorm 0-1
% range need to be represented if the unit of color is nits or 80x nits.
% Image matrices of uint8 format get converted into fp16 format, applying a
% suitable conversion factor from unorm to HDR.
%
% 2. The movie playback functions will try to decode SDR and HDR movies
% into the suitable linear color space used for HDR display, so that they
% display as intended by the movie creator.
%
% More HDR specific processing/treatment may be implemented in the future.
%

% This is the numeric constant for this mode:
rc = 2^24;

return;
