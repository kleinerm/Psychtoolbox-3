function M = RGBToXYZMatrix(xr, yr, xg, yg, xb, yb, xw, yw)
% M = RGBToXYZMatrix(xr, yr, xg, yg, xb, yb, xw, yw)
%
% Build a 3x3 color space conversion (CSC) matrix for converting R,G,B
% linear color value vectors into the XYZ color space, by a matrix-vector
% multiplication, ie. xyz = M * rgb;
%
% The source color space for the rgb (R,G,B) vector is defined by its
% color gamut, which in turn is specified by the 2D CIE-1931 chromaticity
% coordinates of the white-point (xw, yw) and the color primaries red,
% green and blue (xr, yr), (xg, yg) and (xb, yb).
%
% See also the function XYZToRGBMatrix() for building the inverse matrix
% for mapping from XYZ space to a target RGB color space.
%
% This code is based on the mathematical derivations and sample code from
% Gernot Hoffmann, from: http://docs-hoffmann.de/ciexyz29082000.pdf
%
% The math is consistent afaics with the one derived by Bruce Lindbloom:
% http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
%
% Also with http://www.ryanjuckett.com/programming/rgb-color-space-conversion
%
% This Matlab code delivers the same results as Screen()'s internal C
% implementation for color space conversion during movie playback, and was
% also spot-checked against publically available reference matrices for the
% sRGB color space, e.g., Wikipedia, Matlab / Octave, esp. Bruce Lindbloom:
% http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
%
% So there's some confidence this is not totally wrong ;) - But i'm not a
% color scientist, so buyers beware!
%

% History:
%
% 30-Sep-2020   mk  Written.

    zr = 1 - xr - yr;
    zg = 1 - xg - yg;
    zb = 1 - xb - yb;
    zw = 1 - xw - yw;

    W = [xw; yw; zw];
    P = [xr xg xb; yr yg yb; zr zg zb];
    u = P \ W;
    D = diag(u / yw);
    M = P * D;
end
