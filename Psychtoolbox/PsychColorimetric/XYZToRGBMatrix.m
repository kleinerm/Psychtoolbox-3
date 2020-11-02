function M = XYZToRGBMatrix(xr, yr, xg, yg, xb, yb, xw, yw)
% M = XYZToRGBMatrix(xr, yr, xg, yg, xb, yb, xw, yw)
%
% Build a 3x3 color space conversion (CSC) matrix for converting X,Y,Z
% linear color value vectors into a RGB color space, by a matrix-vector
% multiplication, ie. rgb = M * xyz;
%
% The destination color space for the rgb (R,G,B) vector is defined by its
% color gamut, which in turn is specified by the 2D CIE-1931 chromaticity
% coordinates of the white-point (xw, yw) and the color primaries red,
% green and blue (xr, yr), (xg, yg) and (xb, yb).
%
% See also the function RGBToXYZMatrix() for building the inverse matrix
% for mapping from a RGB space to the XYZ color space.
%
% This code just calls RGBToXYZMatrix(xr, yr, xg, yg, xb, yb, xw, yw) and
% then computes the inverse matrix. See the more detailed help text for
% RGBToXYZMatrix() for more infos and background.
%

% History:
%
% 30-Sep-2020   mk  Written.

    M = inv(RGBToXYZMatrix(xr, yr, xg, yg, xb, yb, xw, yw));
end
