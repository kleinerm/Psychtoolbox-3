function [image] = MakeSineImage(freqi,freqj,nRowPixels,nColPixels,centerRowPixel,centerColPixel)
% [image] = MakeSineImage(freqi,freqj,nRowPixels,[nColPixels],[centerRowPixel],[centerColPixel])
%
% Computes a two-dimensional sine function image.
%
% The image has dimensions nRowPixels by nColPixels.
% If nColPixels is omitted, a square image is returned.
%
% If you want to pass centerRowPixel, you must pass nColPixels.
% centerRowPixel and centerColPixel each default to zero if not passed.
%
% 8/15/94		dhb		Both row and column dimensions used if passed.
%				dhb		Changed zero frequency convention.
% 6/20/98       dhb, mw Fixed error in zero handling case.
% 9/07/23       dhb     Allow passing of center row and column pixels,
%                       leaving previous behavior unaffected

% Set column pixels for square image if it wasn't passed.
if (nargin <= 3)
	nColPixels = nRowPixels;
end

if (nargin <= 4)
	centerRowPixel = 0;
    centerColPixel = 0;
end

if (nargin <= 5)
    centerColPixel = 0;
end

x = 1:nColPixels;
y = 1:nRowPixels;
usefreqi = 2*pi*freqi/nRowPixels;
usefreqj = 2*pi*freqj/nColPixels;

% Handle zero frequency case
if (usefreqj == 0 && usefreqi ~= 0)
	sinx = ones(size(x));
else
	sinx = sin(usefreqj*(x-centerColPixel));
end

% Handle zero frequency case
if (usefreqi == 0 && usefreqj ~= 0)
	siny = ones(size(y));
else
	siny = sin(usefreqi*(y-centerRowPixel));
end

% Build composite image
image = siny'*sinx;
