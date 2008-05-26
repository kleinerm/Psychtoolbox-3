function luttex = PsychHelperCreateGenericLuminanceToRGBA8LUT(lut)
% luttex = PsychHelperCreateGenericLuminanceToRGBA8LUT(lut);
%
% Helper function for PsychImaging() - Don't call from usercode!
%
% Used by PsychImaging(....,'EnableGenericHighPrecisionLuminanceOutput', lut);
% Converts a luminance -> RGB8 or RGBA8 LUT as provided in the 3- or 4 rows
% by n columns matrix 'lut' into an equivalent RGBA8 lookup table texture,
% then returns the texture handle to the calling routine.
%
% 'lut' = 3 rows by nslots columns uint8 matrix for LUT's that map the
% input luminance value range [0.0 - 1.0] to the integral 0 - nslots range,
% then lookup the corresponding RGB8 output pixel in the lut. If 'lut' has
% 4 rows, then the 4th row encodes alpha-channel - This is only useful for
% debugging, as teh alpha channel can't be output to the external device.
%
% Implementation details:
%
% The lut texture is built as a 256x256 2D rectangle texture, allowing for
% luts with up to 2^16 luminance slots that work on all supported graphics
% hardware. One could extend the size of the texture to up to 8192x8192 on
% latest hardware, allowing for up to 2^26 bits, but that is likely not
% that useful, given that internal GPU precision is restricted to 23 bits
% (floating point single precision) and there doesn't exist a single output
% device with more than 16 bits of output resolution anyway.
%

% We assume GL is available:
global GL;

if nargin < 1
    error('Called without mandatory "lut" input argument!');
end

if isempty(lut)
    error('Called without mandatory "lut" input argument!');
end

[channels, nslots] = size(lut);

if channels<3 || channels>4
    error('lut must have 3 or 4 rows aka channels!');
end

if nslots < 1 || nslots > 2^16
    error('lut must have between 1 and 65536 slots, aka columns!');
end

if ~isa(lut,'uint8')
    error('lut must by of uint8 type!');
end

% Pad lut with a fourth row of zeros, if it isn't already a 4 row lut:
if channels~=4
    lut = [lut ; uint8(zeros(1, nslots))];
end

nrows = ceil(nslots / 256);
ncols = 256;

% Padding required? Expand lut with 'nexpand' zero-slots aka columns if lut
% size is not a multiple of 'ncols' slots:
if mod(nslots, ncols)~=0
    nexpand = ncols - mod(nslots, ncols);
    lut = [lut , uint8(zeros(4, nexpand))];
end

% Ok, our 'lut' is 4 rows aka channels, and has a multiple of 'ncols' slots
% to cleanly fill 'nrows' rows of a 'nrows' by 'ncols' matrix. Copy it into
% matrix:
clut = uint8(zeros(4, ncols, nrows));

for i=1:nrows
    clut(1,:,i) = lut(1, (((i-1)*ncols + 1):(i*ncols)));
    clut(2,:,i) = lut(2, (((i-1)*ncols + 1):(i*ncols)));
    clut(3,:,i) = lut(3, (((i-1)*ncols + 1):(i*ncols)));
    clut(4,:,i) = lut(4, (((i-1)*ncols + 1):(i*ncols)));
end

% Build texture:
% Create and setup texture from 'clut':
luttex = glGenTextures(1);
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, luttex);
glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, GL.RGBA, ncols, nrows, 0, GL.RGBA, GL.UNSIGNED_BYTE, clut);

% Make sure we use nearest neighbour sampling:
glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_MIN_FILTER, GL.NEAREST);
glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_MAG_FILTER, GL.NEAREST);

% And that we clamp to edge:
glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_S, GL.CLAMP);
glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_T, GL.CLAMP);

glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

% Ready, return luttex:
return;
