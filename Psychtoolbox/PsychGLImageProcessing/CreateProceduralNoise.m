function [noiseid, noiserect] = CreateProceduralNoise(windowPtr, width, height, noiseType, backgroundColorOffset, varargin)
% [noiseid, noiserect] = CreateProceduralNoise(windowPtr, width, height [, noiseType='ClassicPerlin'][, backgroundColorOffset =(0,0,0,0)][, param1][, param2, ...])
%
% Creates a procedural texture that allows to draw random noise stimulus patches
% in a very fast and efficient manner on modern graphics hardware.
%
% This works on GPU's with support for the OpenGL shading language and
% vertex- and fragment shaders. See ProceduralNoiseDemo for examples on how to use this function.
%
%
% Parameters and their meaning:
%
% 'windowPtr' A handle to the onscreen window.
%
%
% 'width' x 'height' The maximum 2D size (in pixels) of the noise patch.
%
%
% 'noiseType' Name of the noise function to use, defaults to 'ClassicPerlin'.
% The following types of noise are currently supported:
%
% - 'Perlin': Generates Perlin noise, which is approximately gaussian
%             distributed.
%
% - 'ClassicPerlin': Like Perlin, but with the classic implementation.
%
%
% 'backgroundColorOffset' Optional, defaults to [0 0 0 0]. A RGBA offset
% color to add to the final RGBA colors of the drawn gabor, prior to
% drawing it.
%
%
% 'param1' ... 'paramN' - Parameters specific to the noise function.
%
%
% The function returns a procedural texture handle 'noiseid' that you can
% pass to the Screen('DrawTexture(s)', windowPtr, noiseid, ...) functions
% like any other texture handle. The 'noiserect' is a rectangle which
% describes the size of the noise texture support.
%
% A typical invocation to draw a single noise patch may look like this:
%
% Screen('DrawTexture', windowPtr, noiseid, [], dstRect, [], [], [],
% modulateColor, [], [], [contrast, seed, 0, 0]);
%
% Draws the patch 'noiseid' into window 'windowPtr', at position 'dstRect',
% or in the center if 'dstRect' is set to []. Make sure 'dstRect' has the
% size of 'noiserect' to avoid spatial distortions! You could do, e.g.,
% dstRect = OffsetRect(noiserect, xc, yc) to place the patch centered at
% screen position (xc,yc).
%
%
% 'modulateColor' is the base color of the noise patch - it defaults to
% white, ie. the noise has only luminance, but no color. If you'd set it to
% [255 0 0] you'd get a reddish noise.
%
%
% 'contrast' is the amplitude of your patch in intensity units - A factor
% that is multiplied to the random number before converting the
% value into a color value. 'contrast' may be a bit of a misleading term
% here...
%
% 'seed' Defines the random seed for drawing of a pseudo-random patch.
%
% Make sure to use the Screen('DrawTextures', ...); function properly,
% instead of the Screen('DrawTexture', ...); function, if you want to draw
% many different patches simultaneously - this is much faster!
%

% History:
% 18.03.2011 Written. Derived from CreateProceduralGabor. (MK)
% 26.02.2012 Updated to use classic perlin noise by default. (MK)

debuglevel = 1;

% Global GL struct: Will be initialized in the LoadGLSLProgramFromFiles
% below:
global GL;

% Make sure we have support for shaders, abort otherwise:
AssertGLSL;

if nargin < 3 || isempty(windowPtr) || isempty(width) || isempty(height)
    error('You must provide "windowPtr", "width" and "height"!');
end

if nargin < 4 || isempty(noiseType)
    noiseType = 'ClassicPerlin';
end

if nargin < 5 || isempty(backgroundColorOffset)
    backgroundColorOffset = [0 0 0 0];
else
    if length(backgroundColorOffset) < 4
        error('The "backgroundColorOffset" must be a 4 component RGBA vector [red green blue alpha]!');
    end
end

noiseShader = [];

if strcmpi(noiseType, 'Perlin')
    % Load standard 2D Perlin noise shader:
    noiseShader = LoadGLSLProgramFromFiles({'PerlinNoiseLib.frag.txt', 'BasicPerlinNoiseShader.vert.txt', 'BasicPerlinNoiseShader.frag.txt'}, debuglevel);
end

if strcmpi(noiseType, 'ClassicPerlin')
    % Load standard 2D Perlin noise shader:
    noiseShader = LoadGLSLProgramFromFiles({'ClassicPerlinNoiseLib.frag.txt', 'BasicPerlinNoiseShader.vert.txt', 'BasicPerlinNoiseShader.frag.txt'}, debuglevel);
end

if isempty(noiseShader)
    error('Unknown/Unsupported noiseType specified or shader load error!');
end

% Setup shader:
glUseProgram(noiseShader);

% Set color offset:
glUniform4f(glGetUniformLocation(noiseShader, 'Offset'), backgroundColorOffset(1),backgroundColorOffset(2),backgroundColorOffset(3),backgroundColorOffset(4));

% Setup done:
glUseProgram(0);

% Create a purely virtual procedural texture 'gaborid' of size width x height virtual pixels.
% Attach the noiseShader to it to define its appearance:
noiseid = Screen('SetOpenGLTexture', windowPtr, [], 0, GL.TEXTURE_RECTANGLE_EXT, width, height, 1, noiseShader);

% Query and return its bounding rectangle:
noiserect = Screen('Rect', noiseid);

% Ready!
return;
