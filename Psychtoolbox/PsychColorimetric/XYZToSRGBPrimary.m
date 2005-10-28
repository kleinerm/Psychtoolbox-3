function [rgb,M] = XYZToSRGBPrimary(XYZ)
% [rgb,M] = XYZToSRGBPrimary(XYZ)
%
% Convert between CIE XYZ to sRGB primary
% coordinates.  These are linear device
% coordinates for the primaries of the sRGB
% standard.  If your input is scaled in the
% gamut of the monitor, the numbers will come
% out in the range 0-1.  You may want to scale
% the result into the range 0-1 before applying
% sRGB gamma correction. 
%
% Conversion matrix as speciedi at:
% http://www.srgb.com/basicsofsrgb.htm
%
% 5/1/04	dhb				Wrote it.

M = [3.2406 -1.5372 -0.4986 ; -0.9689 1.8758 0.0415 ; 0.0557 -0.2040 1.0570];
if (~isempty(XYZ))
    rgb = M*XYZ;
else
    rgb = [];
end


