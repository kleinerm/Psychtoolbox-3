function [discid, discrect] = CreateProceduralSmoothedDisc(windowPtr, width, height, backgroundColorOffset, radius, sigma, useAlpha, method)
% [discid, discrect] = CreateProceduralSmoothedDisc(windowPtr, width, height 
% [, backgroundColorOffset =(0,0,0,0)] [, radius=inf] [, sigma=11] [,useAlpha=1] [,method=1])
%
% Creates a procedural texture that allows to draw smoothed discs
% in a very fast and efficient manner on modern graphics hardware.
%
% 'windowPtr' A handle to the onscreen window.
%
% 'width' x 'height' The maximum size (in pixels) of the grating. More
% precise, the size of the mathematical support of the grating. Providing too
% small values here would 'cut off' peripheral parts or your grating. Too big
% values don't hurt wrt. correctness or accuracy, they just hurt
% performance, ie. drawing speed. Use a reasonable size for your purpose.
%
% 'backgroundColorOffset' Optional, defaults to [0 0 0 0]. A RGBA offset
% color to add to the final RGBA colors of the drawn grating, prior to
% drawing it.
%
% 'radius' If specified, a circular aperture of
% 'radius' pixels is applied to the grating. By default, aperture == width.
%
% 'sigma' edge smoothing value in pixels
%
% 'useAlpha' whether to use colour (0) or alpha (1) for smoothing channel
%
% 'method' whether to use cosine (0) or smoothstep (1) functions. If you
%  pass (2) then the smoothstep will be inverted, so you can use it as a
%  mask (see ProceduralSmoothedDiscMaskDemo.m for example).
%
% The function returns a procedural texture handle that you can
% pass to the Screen('DrawTexture(s)', windowPtr, id, ...) functions
% like any other texture handle. The 'discrect' is a rectangle which
% describes the size of the support.

% History:
% 06/10/2016 Modified from PTB function (iandol).

debuglevel = 1;

% Global GL struct: Will be initialized in the LoadGLSLProgramFromFiles
% below:
global GL;

if nargin < 3 || isempty(windowPtr) || isempty(width) || isempty(height)
    error('You must provide "windowPtr", "width" and "height"!');
end

if nargin < 4 || isempty(backgroundColorOffset)
    backgroundColorOffset = [0 0 0 0];
else
    if length(backgroundColorOffset) < 4
        error('The "backgroundColorOffset" must be a 4 component RGBA vector [red green blue alpha]!');
    end
end

if nargin < 5 || isempty(radius)
    radius = width;
end

if nargin < 6 || isempty(sigma)
    sigma = 11.0;
end

if nargin < 7 || isempty(useAlpha)
    useAlpha = 1.0;
end

if nargin < 8 || isempty(method)
    method = 1.0;
end

% Switch to windowPtr OpenGL context:
Screen('GetWindowInfo', windowPtr);

% Make sure we have support for shaders, abort otherwise:
AssertGLSL;

% Load shader with circular aperture and smoothing support:
discShader = LoadGLSLProgramFromFiles('SmoothedDiscShader', debuglevel);

% Setup shader:
glUseProgram(discShader);

% Set the 'Center' parameter to the center position of the gabor image
% patch [tw/2, th/2]:
glUniform2f(glGetUniformLocation(discShader, 'Center'), width/2, height/2);
glUniform4f(glGetUniformLocation(discShader, 'Offset'), backgroundColorOffset(1),backgroundColorOffset(2),backgroundColorOffset(3),backgroundColorOffset(4));

if ~isinf(radius)
    % Set radius of circular aperture:
    glUniform1f(glGetUniformLocation(discShader, 'Radius'), radius);
    % Apply sigma:
    glUniform1f(glGetUniformLocation(discShader, 'Sigma'), sigma);
    % Apply useAlpha:
    glUniform1f(glGetUniformLocation(discShader, 'useAlpha'), useAlpha);
    % Apply method:
    glUniform1f(glGetUniformLocation(discShader, 'Method'), method);
end

glUseProgram(0);

% Create a purely virtual procedural texture 'discid' of size width x height virtual pixels.
% Attach the discShader to it to define its appearance:
discid = Screen('SetOpenGLTexture', windowPtr, [], 0, GL.TEXTURE_RECTANGLE_EXT, width, height, 1, discShader);

% Query and return its bounding rectangle:
discrect = Screen('Rect', discid);

% Ready!
return;
