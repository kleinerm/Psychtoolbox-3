function textureId = moglMakeHDRTexture(win, hdrImage, halffloat, poweroftwo)
% textureId = moglMakeHDRTexture(win, hdrImage [, halffloat][, poweroftwo])
%
% Create a high dynamic range Psychtoolbox texture from the given
% Matlab/Octave high dynamic range image matrix 'hdrImage' and attach
% it to the onscreen window 'win'.
%
% Before calling this function you must have called Screen('BeginOpenGL', win);
% with 'win' being the window handle for the onscreen window to
% which the texture should be attached.
%
% Returns a Psychtoolbox handle 'textureId' for the created 2D texture.
% The texture can be used like any other Psychtoolbox texture, just
% with the difference that it represents its color values with 32 bit
% floating point precision or 16 bit half floating point precision
% instead of 8 bpc fixed point precision.
%
% The optional flag 'halffloat', if set to 1, will trigger creation of a
% texture in half float format, ie. 16 bit floating point numbers instead
% of 32 bit ones. This saves 50% memory and bandwidth. It also allows to
% apply bilinear filtering during texture blits in hardware on GeForce 6000
% and higher.
%
% Normally, Psychtoolbox will try to select a GL_TEXTURE_RECTANGLE
% texture if the hardware supports it. You can enforce creation of
% a power-of-two GL_TEXTURE_2D by setting the optional 'poweroftwo'
% flag to a value of 1.
%
% 'hdrImage' must be a (height, width, 4) matrix of type 'double' or
% 'single', where channel 1=Red, 2=Green, 3=Blue, 4=Alpha. The numeric
% range (0.0 - 1.0) maps to (minimum intensity - maximum intensity).
%
% If you want to use the created 2D texture for 3D OpenGL rendering
% as well, you can use the Screen('GetOpenGLTexture') function to
% retrieve a standard OpenGL texture handle to it.
%
% If you don't want to create a pure 2D texture, but a cube map texture
% for use in 3D environment mapped lighting and such, then use the
% function moglMakeGLHDRTexture() instead. It can create cube-map
% textures for use with the OpenGL functions. These are not useable
% with the standard Psychtoolbox Screen() commands.
%

% History:
% 7.8.2006 Created (MK)

global GL;

if isempty(GL)
   InitializeMatlabOpenGL;
end;

if nargin < 1
   error('Required window handle ''win'' is missing.');
end

if nargin < 2
   error('Required input ''hdrImage'' matrix is missing.');
end

if nargin < 3
   halffloat = 0;
end

if nargin < 4
   poweroftwo = 0;
end

if poweroftwo == 1
   gltextarget = GL.TEXTURE_2D;
else
   gltextarget = GL.TEXTURE_RECTANGLE_EXT;
end

% Call the routine that creates an OpenGL texture:
gltexid = moglMakeGLHDRTexture(hdrImage, gltextarget, halffloat);

% Assign the glTexture to Psychtoolbox:
textureId = Screen('SetOpenGLTexture', win, [], gltexid, gltextarget, size(hdrImage, 2), size(hdrImage, 1), 32);

% Return.
return;
