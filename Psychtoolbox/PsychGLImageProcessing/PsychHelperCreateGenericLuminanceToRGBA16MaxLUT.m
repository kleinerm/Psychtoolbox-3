function luttex = PsychHelperCreateGenericLuminanceToRGBA16MaxLUT(lut, targetBpc, win)
% luttex = PsychHelperCreateGenericLuminanceToRGBA16MaxLUT(lut, targetBpc, win);
%
% Helper function for PsychImaging() - Don't call from usercode!
%
% Used by PsychImaging(....,'EnableGenericHighPrecisionLuminanceOutput', lut);
% Converts a uint16 luminance -> RGB8/10/16 or RGBA8/10/16 LUT as provided in
% the 3- or 4 rows by n columns matrix 'lut' into an equivalent RGBA8/10/16
% lookup table texture, then returns the texture handle to the calling routine.
%
% 'lut' = 3 rows by nslots columns uint8 matrix for LUT's that map the
% input luminance value range [0.0 - 1.0] to the integral 0 - nslots range,
% then lookup the corresponding RGB8/10/16 output pixel in the lut. If 'lut' has
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

if channels < 3 || channels > 4
    error('lut must have 3 or 4 rows aka channels!');
end

if nslots < 1 || nslots > 2^16
    error('lut must have between 1 and 65536 slots, aka columns!');
end

if ~isa(lut,'uint16')
    error('lut must by of uint16 type!');
end

% Pad lut with a fourth row of zeros, if it isn't already a 4 row lut:
if channels~=4
    lut = [lut ; uint16(zeros(1, nslots))];
end

nrows = ceil(nslots / 256);
ncols = 256;

% Padding required? Expand lut with 'nexpand' zero-slots aka columns if lut
% size is not a multiple of 'ncols' slots:
if mod(nslots, ncols)~=0
    nexpand = ncols - mod(nslots, ncols);
    lut = [lut , uint16(zeros(4, nexpand))];
end

% Ok, our 'lut' is 4 rows aka channels, and has a multiple of 'ncols' slots
% to cleanly fill 'nrows' rows of a 'nrows' by 'ncols' matrix. Copy it into
% matrix:
clut = uint16(zeros(4, ncols, nrows));

for i=1:nrows
    clut(1,:,i) = lut(1, (((i-1)*ncols + 1):(i*ncols)));
    clut(2,:,i) = lut(2, (((i-1)*ncols + 1):(i*ncols)));
    clut(3,:,i) = lut(3, (((i-1)*ncols + 1):(i*ncols)));
    clut(4,:,i) = lut(4, (((i-1)*ncols + 1):(i*ncols)));
end

% Select format with minimal memory storage requirement for faithful
% representation of targetBpc bits deep RGB color:
if targetBpc <= 8
    % Classic 8 bpc:
    internalFormat = GL.RGBA;
elseif targetBpc <= 10
    % Up to 10 bpc:
    internalFormat = GL.RGB10_A2;
else
    % Up to 16 bpc:
    internalFormat = GL.RGBA16;
end

% Build texture:
% Create and setup texture from 'clut':
luttex = glGenTextures(1);
glBindTexture(GL.TEXTURE_RECTANGLE_EXT, luttex);

% Allocate texture storage:
glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, internalFormat, ncols, nrows, 0, GL.RGBA, GL.UNSIGNED_SHORT, clut);

% Query what we've got:
trueR = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_RED_SIZE_EXT);
trueG = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_GREEN_SIZE_EXT);
trueB = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_BLUE_SIZE_EXT);

if (internalFormat == GL.RGBA) && (trueR < 8 || trueG < 8 || trueB < 8)
    warning('PsychHelperCreateGenericLuminanceToRGBA16MaxLUT: Did not even get required 8 bpc LUT depth!');
end

if (internalFormat == GL.RGB10_A2) && (trueR < targetBpc || trueG < targetBpc || trueB < targetBpc)
    fprintf('PsychHelperCreateGenericLuminanceToRGBA16MaxLUT: Did not get required %i bpc LUT depth, upgrading to 16 bpc texture.\n', targetBpc);
    internalFormat = GL.RGBA16;
    glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, internalFormat, ncols, nrows, 0, GL.RGBA, GL.UNSIGNED_SHORT, clut);

    % Query what we've got:
    trueR = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_RED_SIZE_EXT);
    trueG = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_GREEN_SIZE_EXT);
    trueB = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_BLUE_SIZE_EXT);
end

if (internalFormat == GL.RGBA16) && (trueR < targetBpc || trueG < targetBpc || trueB < targetBpc)
    fprintf('PsychHelperCreateGenericLuminanceToRGBA16MaxLUT: Did not get required %i bpc LUT depth, upgrading to 32 bpc floating point texture.\n', targetBpc);
    internalFormat = GL.RGBA32F;

    winfo = Screen('GetWindowInfo', win);
    if winfo.GLSupportsTexturesUpToBpc < 32
        error('PsychHelperCreateGenericLuminanceToRGBA16MaxLUT: Would need to use 32 bit floating point textures, but these are unsupported by the hardware!');
    end

    glTexImage2D(GL.TEXTURE_RECTANGLE_EXT, 0, internalFormat, ncols, nrows, 0, GL.RGBA, GL.UNSIGNED_SHORT, clut);

    % Query what we've got:
    trueR = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_RED_SIZE_EXT);
    trueG = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_GREEN_SIZE_EXT);
    trueB = glGetTexLevelParameteriv(GL.TEXTURE_RECTANGLE_EXT, 0, GL.TEXTURE_BLUE_SIZE_EXT);

    if (trueR < targetBpc || trueG < targetBpc || trueB < targetBpc)
        error('PsychHelperCreateGenericLuminanceToRGBA16MaxLUT: Impossible to get LUT textures of sufficient precision from hardware!');
    end
end

% Ok, we have a texture of sufficient but minimal bit depths for driving a targetBpc deep framebuffer, suitable for use as a LUT.
switch internalFormat
    case GL.RGBA
        fprintf('PsychHelperCreateGenericLuminanceToRGBA16MaxLUT: Using RGBA8 LUT with bit depths [%i,%i,%i] for targetBpc %i\n', trueR, trueG, trueB, targetBpc);
    case GL.RGB10_A2
        fprintf('PsychHelperCreateGenericLuminanceToRGBA16MaxLUT: Using RGB10_A2 LUT with bit depths [%i,%i,%i] for targetBpc %i\n', trueR, trueG, trueB, targetBpc);
    case GL.RGBA16
        fprintf('PsychHelperCreateGenericLuminanceToRGBA16MaxLUT: Using RGBA16 LUT with bit depths [%i,%i,%i] for targetBpc %i\n', trueR, trueG, trueB, targetBpc);
    case GL.RGBA32F
        fprintf('PsychHelperCreateGenericLuminanceToRGBA16MaxLUT: Using RGBA32F floating point LUT with bit depths [%i,%i,%i] for targetBpc %i\n', trueR, trueG, trueB, targetBpc);
end

% Make sure we use nearest neighbour sampling:
glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_MIN_FILTER, GL.NEAREST);
glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_MAG_FILTER, GL.NEAREST);

% And that we clamp to edge:
glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_S, GL.CLAMP);
glTexParameteri(GL.TEXTURE_RECTANGLE_EXT, GL.TEXTURE_WRAP_T, GL.CLAMP);

glBindTexture(GL.TEXTURE_RECTANGLE_EXT, 0);

% Ready, return luttex:
return;
